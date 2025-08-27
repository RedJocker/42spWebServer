/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 10:51:33 by vcarrara          #+#    #+#             */
/*   Updated: 2025/08/27 12:24:43 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "TcpClient.hpp"
#include "BufferedReader.hpp"
#include <sstream>
#include <cstdlib>
#include <cctype>

namespace http
{
	Request::Request(void)
		: _method()
		, _path()
		, _protocol()
		, _headers()
		, _body()
		, _readBuffer()
		, _state(READING_REQUEST_LINE)
		, _expectedBodyLength(0)
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
			this->_readBuffer = other._readBuffer;
			this->_state = other._state;
			this->_expectedBodyLength = other._expectedBodyLength;
		}
		return *this;
	}

	Request::~Request(void) {}

	Request::ReadState Request::readFromTcpClient(conn::TcpClient &client) {
		switch (_state) {
			case READING_REQUEST_LINE: {
				std::pair<BufferedReader::ReadState, char*> result = client.readlineCrlf();

				if (!result.second || result.first == BufferedReader::READING || result.first == BufferedReader::NO_CONTENT)
					return _state;
				if (result.first == BufferedReader::ERROR) {
					_state = READ_ERROR;
					return _state;
				}

				std::string line(result.second);
				delete[] result.second;

				std::istringstream lineStream(line);
				if (!(lineStream >> _method >> _path >> _protocol)) {
					_state = READ_ERROR;
					return _state;
				}

				_state = READING_HEADERS;
				return _state;
			}

			case READING_HEADERS: {
				std::pair<BufferedReader::ReadState, char*> result = client.readlineCrlf();

				if (!result.second || result.first == BufferedReader::READING || result.first == BufferedReader::NO_CONTENT)
					return _state;
				if (result.first == BufferedReader::ERROR) {
					_state = READ_ERROR;
					return _state;
				}

				std::string line(result.second);
				delete[] result.second;

				if (line.empty() || line == "\n" || line == "\r\n") {
					std::map<std::string, std::string>::iterator it = _headers.find("Content-Length");

					if (it != _headers.end()) {
						_expectedBodyLength = strtoul(it->second.c_str(), NULL, 10);
						_state = (_expectedBodyLength > 0) ? READING_BODY : READ_COMPLETE;
					}
					else
						_state = READ_COMPLETE;

					return _state;
				}

				// Trimming CRLF
				if (line.size() >= 2 && line[line.size() - 2] == '\r' && line[line.size() - 1] == '\n')
					line.erase(line.size() - 2);
				else if (!line.empty() && (line[line.size() - 1] == '\n' || line[line.size() - 1] == '\r'))
					line.erase(line.size() - 1);

				size_t colon = line.find(':');
				if (colon == std::string::npos) {
					_state = READ_ERROR;
					return _state;
				}

				std::string key = line.substr(0, colon);
				std::string value = line.substr(colon + 1);
				while (!value.empty() && std::isspace(value[0]))
					value.erase(0, 1);

				_headers[key] = value;
				return _state;
			}

			case READING_BODY: {
				std::pair<BufferedReader::ReadState, char*> chunk = client.read(_expectedBodyLength - _body.size());

				if (!chunk.second || chunk.first == BufferedReader::READING || chunk.first == BufferedReader::NO_CONTENT)
					return _state;
				if (chunk.first == BufferedReader::ERROR) {
					_state = READ_ERROR;
					delete[] chunk.second;
					return _state;
				}

				_body.append(chunk.second);
				delete[] chunk.second;

				if (_body.size() >= _expectedBodyLength)
					_state = READ_COMPLETE;

				return _state;
			}

			case READ_ERROR:
			case READ_COMPLETE:
				return _state;

			return READ_ERROR;
		}
	}

	std::string Request::getMethod() const { return _method; }
	std::string Request::getPath() const { return _path; }
	std::string Request::getProtocol() const { return _protocol; }
	std::string Request::getHeader(const std::string &key) const {
		std::map<std::string, std::string>::const_iterator it = _headers.find(key);
		return it != _headers.end() ? it->second : "";
	}
	std::string Request::getBody() const { return _body; }
}
