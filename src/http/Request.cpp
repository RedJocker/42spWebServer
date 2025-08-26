/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 10:51:33 by vcarrara          #+#    #+#             */
/*   Updated: 2025/08/26 10:58:32 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <unistd.h>
#include <sstream>
#include <iostream>

namespace http
{
	Request::Request(void) {
	}

	Request::Request(const Request &other) {
		*this = other;
	}

	Request &Request::operator=(const Request &other) {
		if (this != &other) {
			this->_method = other._method;
			this->_path = other._path;
			this->_protocol = other._protocol;
		}
		return *this;
	}

	Request::~Request(void) {
	}

	static bool	readLineFromFd(int fd, std::string &out) {
		out.clear();
		char ch = 0;
		bool sawCR = false;

		while (true) {
			ssize_t	n = ::read(fd, &ch, 1);
			if (n <= 0)
				return false; // Error before EOL or EOF
			if (ch == '\r') {
				sawCR = true;
				continue; // Expecting \r\n
			}
			if (ch == '\n')
				return true; // EOL
			if (sawCR) {
				out.push_back(ch);
				return true; // First line only
			}
			out.push_back(ch);
		}
	}

	bool Request::parseFromFd(int fd) {
		std::string requestLine;
		if (!readLineFromFd(fd, requestLine))
			return false; // EOF or error

		std::istringstream rl(requestLine);
		if (!(rl >> _method >> _path >> _protocol))
			return false; // Failed to populate

		return true;
	}

	std::string Request::getMethod() const { return _method; }
	std::string Request::getPath() const { return _path; }
	std::string Request::getProtocol() const { return _protocol; }
}
