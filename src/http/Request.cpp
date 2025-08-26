/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 10:51:33 by vcarrara          #+#    #+#             */
/*   Updated: 2025/08/26 14:11:13 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <cctype>
#include <cstdlib>

namespace http
{
	Request::Request(void) {}
	Request::Request(const Request &other) { *this = other; }
	Request &Request::operator=(const Request &other) {
		if (this != &other) {
			this->_method = other._method;
			this->_path = other._path;
			this->_protocol = other._protocol;
			this->_headers = other._headers;
			this->_body = other._body;
			this->_readBuffer = other._readBuffer;
		}
		return *this;
	}
	Request::~Request(void) {}

	bool Request::readFromFd(int fd) {
		char tmp[4096];
		ssize_t n = ::read(fd, tmp, sizeof(tmp));
		if (n <= 0) {
			if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
				return false; // No data available
			return false; // Error or EOF
		}

		_readBuffer.append(tmp, n);

		// Parse request line first
		size_t lineEnd = _readBuffer.find("\r\n");
		if (lineEnd == std::string::npos) {
			lineEnd = _readBuffer.find("\n"); // LF-only
			if (lineEnd == std::string::npos)
				return false; // Request line incomplete
		}

		std::string line = _readBuffer.substr(0, lineEnd);
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		std::istringstream lineStream(line);
		if (!(lineStream >> _method >> _path >> _protocol))
			return false; // Bad request line

		_readBuffer.erase(0, lineEnd + (lineEnd + 2 <= _readBuffer.size() && _readBuffer[lineEnd] == '\r' && _readBuffer[lineEnd + 1] == '\n' ? 2 : 1)); // remove processed line

		// Check if there are headers and body
		size_t headersEnd = _readBuffer.find("\r\n\r\n");
		if (headersEnd == std::string::npos)
			headersEnd = _readBuffer.find("\n\n"); // LF-only
		if (headersEnd != std::string::npos) {
			std::istringstream headersStream(_readBuffer.substr(0, headersEnd));
			std::string headerLine;
			while (std::getline(headersStream, headerLine)) {
				if (!headerLine.empty() && headerLine[headerLine.size() - 1] == '\r')
					headerLine.erase(headerLine.size() - 1);
				if (headerLine.empty())
					break;

				size_t colon = headerLine.find(':');
				if (colon != std::string::npos) {
					std::string key = headerLine.substr(0, colon);
					std::string value = headerLine.substr(colon + 1);
					while (!value.empty() && value[0] == ' ')
						value.erase(0, 1); // Trim leading spaces
					_headers[key] = value;
				}
			}
			_readBuffer.erase(0, headersEnd + 4); // Remove headers from buffer

			// Read body if Content-Length set
			std::map<std::string, std::string>::iterator it;
			for (it = _headers.begin(); it != _headers.end(); ++it) {
				if (it->first == "Content-Length") {
					size_t contentLength = strtoul(it->second.c_str(), NULL, 10);
					if (_readBuffer.size() < contentLength)
						return false; // Body not fully received yet
					_body = _readBuffer.substr(0, contentLength);
					_readBuffer.erase(0, contentLength);
					break;
				}
			}
		}

		return true; // Successfully parsed
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
