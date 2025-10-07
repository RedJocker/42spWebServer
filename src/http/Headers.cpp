/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:47:25 by vcarrara          #+#    #+#             */
//   Updated: 2025/10/02 15:32:21 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Headers.hpp"
#include <cctype>
#include <sstream>

namespace http {
	Headers::Headers(void) {}
	Headers::Headers(const Headers &other) {
		this->_headers = other._headers;
	}
	Headers &Headers::operator=(const Headers &other) {
		if (this != &other)
			this->_headers = other._headers;
		return *this;
	}
	Headers::~Headers(void)
	{
		this->clear();
	}

	// Single header line
	bool Headers::parseLine(const std::string &line) {
		size_t colon = line.find(':');
		if (colon == std::string::npos)
			return false;

		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);

		// Trim whitespace
		while (!value.empty() && std::isspace(value[0]))
			value.erase(0, 1);
		while (!value.empty() && std::isspace(value[value.size() - 1]))
			value.erase(value.size() - 1, 1);

		_headers[key] = value;
		return true;
	}

	bool Headers::addHeader(const std::string &key, const std::string &value) {
		_headers[key] = value;
		return true;
	}

	std::map<std::string, std::string> Headers::getAll(void) const {
		return this->_headers;
	}
	
	// Get header value from key
	std::string Headers::getHeader(const std::string &key) const {
		std::map<std::string, std::string>::const_iterator it = _headers.find(key);
		if (it != _headers.end())
			return it->second;
		return "";
	}

	void Headers::clear(void) {
		this->_headers.clear();
	}

	std::string Headers::str(void) const
	{
		std::ostringstream headerStream;
		for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
			 it != _headers.end();
			 ++it) {
			headerStream << it->first << ": " << it->second << "\r\n";
		}
		return headerStream.str();
	}
}
