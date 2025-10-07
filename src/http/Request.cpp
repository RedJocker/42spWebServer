/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 10:51:33 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/07 01:12:46 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "TcpClient.hpp"
#include "BufferedReader.hpp"
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <cstring>

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

	Request::ReadState Request::readRequestLine(BufferedReader &client)
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

	Request::ReadState Request::readHeaderLine(BufferedReader &reader)
	{
		std::pair<BufferedReader::ReadState, char*> result;

		result = reader.readlineCrlf();

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
			if (reader.hasBufferedContent()) {
				if (_state == READING_HEADERS)
					return readHeaderLine(reader);
 				else if (_state == READING_BODY)
					return readBody(reader);
			}
			return _state;
		}
		default:
			return _state;
		}
	}

	Request::ReadState Request::readBody(BufferedReader &reader)
	{
		std::pair<BufferedReader::ReadState, char*> result;

		std::string contentLength = _headers.getHeader("Content-Length");
		size_t expectedLength = 0;
		if (!contentLength.empty()) {
			expectedLength = std::strtoul(contentLength.c_str(), NULL, 10);
		}

		result = reader.read(expectedLength);
		switch(result.first) {
		case BufferedReader::READING:
			return _state;
		case BufferedReader::ERROR: {
			std::cout << "error:" << result.second << std::endl;
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
	

	Request::ReadState Request::readHttpRequest(BufferedReader &reader) {
		switch (_state) {
		case READING_REQUEST_LINE:
			return readRequestLine(reader);
		case READING_HEADERS: 
			return readHeaderLine(reader);
		case READING_BODY:
			return readBody(reader);
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

	std::string Request::toString() const
	{
		std::ostringstream requestStream;
		requestStream << _method << " " << _path << " " << _protocol << "\r\n";
		requestStream << _headers.str();
		requestStream << _body.str();
		return requestStream.str();
	}

	char **Request::envp() const
	{
		std::vector<std::string> envp;
		this->envpInit(envp);

		size_t envp_len = envp.size() + 1;
		char **envp_on_heap = new char*[envp_len];

		for (size_t i = 0; i < envp_len - 1; ++i)
		{
			size_t strSize = envp.at(i).size() + 1;
			char *str = new char[strSize];
			::strncpy(str, envp.at(i).c_str(), strSize);
			str[strSize - 1] = '\0';
			envp_on_heap[i] = str;
		}
		envp_on_heap[envp_len - 1] = reinterpret_cast<char *>(0);
		return envp_on_heap;
	}

	void Request::envpInit(std::vector<std::string> &envp) const
	{
	    // TODO fill envp with variables for cgi process
	    // taken from request data

	    //// headers required for all cgi request
			envp.push_back("REQUEST_METHOD=" + this->_method); // take from request method
			envp.push_back("REDIRECT_STATUS=0"); // always 0?
	    	envp.push_back("SCRIPT_FILENAME=./www/todo.cgi"); // build from request path and docroot
	    ////

	    /// headers required for cgi request with body (body is passed by parent on stdin)
	    	envp.push_back("CONTENT_TYPE=application/x-www-form-urlencoded"); // take from request header
			envp.push_back("CONTENT_LENGTH=19"); // take from request header
		////

		//// headers required for passing query string
			envp.push_back("QUERY_STRING=hello=there&yyy=xxx"); //  build from request path
		////
	}
}
