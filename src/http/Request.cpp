/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 10:51:33 by vcarrara          #+#    #+#             */
//   Updated: 2025/08/29 02:04:49 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "TcpClient.hpp"
#include "BufferedReader.hpp"
#include <sstream>
#include <cstdlib>

namespace http
{
	Request::Request(void)
		: _method()
		, _path()
		, _protocol()
		, _headers()
		, _body()
		, _state(READING_REQUEST_LINE)
	{}

	Request::Request(const Request &other) {
		*this = other;
	}

	Request &Request::operator=(const Request &other) {
		if (this != &other) {
			this->_method = other._method;
			this->_path = other._path;
			this->_protocol = other._protocol;
			this->_headers = other._headers;
			this->_body = other._body;
			this->_state = other._state;
		}
		return *this;
	}

	Request::~Request(void) {}

	bool Request::parseRequestLine(const std::string &line) {
		std::istringstream lineStream(line);
		if (!(lineStream >> _method >> _path >> _protocol))
			return false;
		return true;
	}

	Request::ReadState Request::readRequestLine(conn::TcpClient &client)
	{
		std::pair<BufferedReader::ReadState, char*> result;
		result = client.readlineCrlf();

		switch(result.first) {
		case BufferedReader::READING: {
			return _state;
		}
		case BufferedReader::ERROR: {
			_state = READ_ERROR;
			return _state;
		}
		case BufferedReader::NO_CONTENT: {
			delete[] result.second;
			_state = READ_EOF;
			return _state;
		}
		case BufferedReader::DONE: {
			if (!parseRequestLine(std::string(result.second)))
			{
				_state = READ_BAD_REQUEST;
				delete[] result.second;
				return _state;
			}
			delete[] result.second;
			_state = READING_HEADERS;
			if (client.hasBufferedContent())
				return this->readHeaderLine(client);
			else
				return _state;
		}
		default: return _state;
		}
	}

	Request::ReadState Request::readHeaderLine(conn::TcpClient &client)
	{
		std::pair<BufferedReader::ReadState, char*> result;

		result = client.readlineCrlf();

		switch(result.first) {
		case BufferedReader::READING:
			return _state;
		case BufferedReader::ERROR: {
			_state = READ_ERROR;
			return _state;
		}
		case BufferedReader::NO_CONTENT: {
			delete[] result.second;
			_state = READ_EOF;
			return _state;
		}
		case BufferedReader::DONE:
		{
			std::string line(result.second);
			delete[] result.second;

			if (line.empty()) { // End of Headers
				std::string contentLength = _headers.getHeader("Content-Length");
				if (!contentLength.empty())
					_state = READING_BODY; // Body to read
				else
					_state = READ_COMPLETE; // No body to read
			}
			else if (!_headers.parseLine(line)) { // unexpected format
				_state = READ_BAD_REQUEST;
			}
			if (client.hasBufferedContent()) {
				if (_state == READING_HEADERS)
					return readHeaderLine(client);
 				else if (_state == READING_BODY)
					return readBody(client);
			}
			return _state;
		}
		default:
			return _state;
		}
	}

	Request::ReadState Request::readBody(conn::TcpClient &client)
	{
		std::pair<BufferedReader::ReadState, char*> result;

		std::string contentLength = _headers.getHeader("Content-Length");
		size_t expectedLength = 0;
		if (!contentLength.empty()) {
			expectedLength = std::strtoul(contentLength.c_str(), NULL, 10);
		}

		result = client.read(expectedLength - _body.size());
		switch(result.first) {
		case BufferedReader::READING:
			return _state;
		case BufferedReader::ERROR: {
			_state = READ_ERROR;
			return _state;
		}
		case BufferedReader::NO_CONTENT: {
			_state = READ_EOF;
			return _state;
		}
		case BufferedReader::DONE: {
			if (!_body.parse(result.second, expectedLength)) {
				delete[] result.second;
				return _state;
			}
			delete[] result.second;

			// Check if the body has been fully read
			if (_body.size() >= expectedLength) {
				_state = READ_COMPLETE;
			}
			return _state;
		}
		default:
			return _state;
		}
	}
	

	Request::ReadState Request::readHttpRequest(conn::TcpClient &client) {
		switch (_state) {
		case READING_REQUEST_LINE:
			return readRequestLine(client);
		case READING_HEADERS: 
			return readHeaderLine(client);
		case READING_BODY:
			return readBody(client);
		case READ_BAD_REQUEST:
		case READ_EOF:
		case READ_ERROR:
		case READ_COMPLETE:
			return _state;
		default:
			return _state;
		}
	}

	std::string Request::getMethod() const { return _method; }
	std::string Request::getPath() const { return _path; }
	std::string Request::getProtocol() const { return _protocol; }
	std::string Request::getHeader(const std::string &key) const {
		return _headers.getHeader(key);
	}
	std::string Request::getBody() const { return _body.str(); }
	void Request::clear()
	{
		_method = "";
		_path = "";
		_protocol = "";
		_state = READING_REQUEST_LINE;
		_headers.clear();
		_body.clear();
	}
}
