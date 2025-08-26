/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 10:51:33 by vcarrara          #+#    #+#             */
/*   Updated: 2025/08/26 11:57:12 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <cerrno>

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
			this->_readBuffer = other._readBuffer;
		}
		return *this;
	}
	Request::~Request(void) {}

	bool Request::readFromFd(int fd) {
		char	tmp[4096];
		ssize_t	n = ::read(fd, tmp, sizeof(tmp));
		if (n == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return false; // No data available
			return false; // Error
		}
		if (n == 0)
			return false; // EOF, connection closed

		_readBuffer.append(tmp, n);

		size_t	pos = _readBuffer.find("\r\n\r\n");
		if (pos == std::string::npos)
			return false; // Incomplete request line or headers

		std::istringstream stream(_readBuffer.substr(0, pos));
		std::string line;

		if (!std::getline(stream, line))
			return false; // Failed to read request line
		if (!line.empty() && line.back() == '\r')
			line.pop_back(); // Remove trailing CR
		std::istringstream lineStream(line);
		if (!(lineStream >> _method >> _path >> _protocol))
			return false; // Failed to parse request line

		while (std::getline(stream, line)) {
			if (!line.empty() && line.back() == '\r')
				line.pop_back();
			if (line.empty())
				break; // End of headers

			size_t colon = line.find(':');
			if (colon != std::string::npos) {
				std::string key = line.substr(0, colon);
				std::string value = line.substr(colon + 1);
				while (!value.empty() && value[0] == ' ')
					value.erase(0, 1); // Trim leading spaces
				_headers[key] = value;
			}
		}
		_readBuffer.erase(0, pos + 4); // Remove processed part

		return true; // Successfully parsed
	}

	std::string Request::getMethod() const { return _method; }
	std::string Request::getPath() const { return _path; }
	std::string Request::getProtocol() const { return _protocol; }
	std::string Request::getHeader(const std::string &key) const {
		std::map<std::string, std::string>::const_iterator it = _headers.find(key);
		return it != _headers.end() ? it->second : "";
	}
}
