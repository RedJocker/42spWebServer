/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:47:25 by vcarrara          #+#    #+#             */
//   Updated: 2025/11/18 08:11:46 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Headers.hpp"
#include <cctype>
#include <sstream>
#include "pathUtils.hpp"

namespace http {
	bool Headers::CaseInsensitive::operator()(const std::string &s1, const std::string &s2) const
	{
		const char *str1 = s1.c_str();
		const char *str2 = s2.c_str();
		int i = 0;
		char ch1 = ::tolower(str1[i]);
		char ch2 = ::tolower(str2[i]);
		while (str1[i] && str2[i] && ch1 == ch2)
		{
			++i;
			ch1 = ::tolower(str1[i]);
			ch2 = ::tolower(str2[i]);
		}
		return ch1 < ch2;
	}

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

		utils::trimInPlace(value);

		_headers[key] = value;
		return true;
	}

	bool Headers::addHeader(const std::string &key, const std::string &value) {
		_headers[key] = value;
		return true;
	}

	void Headers::eraseHeader(const std::string &key) {
		_headers.erase(key);
	}

	std::map<std::string, std::string, Headers::CaseInsensitive> Headers::getAll(void) const {
		return this->_headers;
	}
	
	// Get header value from key
	std::string Headers::getHeader(const std::string &key) const {
		std::map<std::string, std::string, Headers::CaseInsensitive>::const_iterator it = _headers.find(key);
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
