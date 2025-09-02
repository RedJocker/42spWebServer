/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:47:25 by vcarrara          #+#    #+#             */
/*   Updated: 2025/09/02 11:42:16 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Headers.hpp"
#include <cctype>

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
	Headers::~Headers(void) {}

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

	// Get header value from key
	std::string Headers::getHeader(const std::string &key) const {
		std::map<std::string, std::string>::const_iterator it = _headers.find(key);
		if (it != _headers.end())
			return it->second;
		return "";
	}

	// Set or update header key-value pair
	void Headers::setHeader(const std::string &key, const std::string &value) {
		_headers[key] = value;
	}

	// Check if header key exists
	bool Headers::has(const std::string &key) const {
		return _headers.find(key) != _headers.end();
	}

	// Convert all headers to string format
	std::string Headers::toString(void) const {
		std::string result;
		for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
			result += it->first + ": " + it->second + "\r\n";
		}
		return result;
	}
}
