/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 13:22:28 by vcarrara          #+#    #+#             */
/*   Updated: 2025/09/02 14:32:23 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sstream>

namespace http {
	Response::Response(void)
	: _protocol("HTTP/1.1"), _statusCode(200), _statusInfo("OK"), _headers(), _body()
	{}
	Response::Response(const std::string &protocol, int statusCode, const std::string &statusInfo)
		: _protocol(protocol), _statusCode(statusCode), _statusInfo(statusInfo), _headers(), _body()
	{}
	Response::Response(const Response &other)
	{
		*this = other;
	}
	Response &Response::operator=(const Response &other)
	{
		if (this != &other) {
			_protocol = other._protocol;
			_statusCode = other._statusCode;
			_statusInfo = other._statusInfo;
			_headers = other._headers;
			_body = other._body;
		}
		return *this;
	}
	Response::~Response(void) {}

	void Response::setProtocol(const std::string &protocol) {
		_protocol = protocol;
	}
	void Response::setStatusCode(int code) {
		_statusCode = code;
	}
	void Response::setStatusInfo(const std::string &info) {
		_statusInfo = info;
	}

	void Response::addHeader(const std::string &key, const std::string &value) {
		_headers.parseLine(key + ": " + value);
	}

	void Response::setBody(const Body &body) {
		_body = body;
	}

	std::string Response::toString(void) const {
		std::ostringstream responseStream;
		responseStream << _protocol << " " << _statusCode << " " << _statusInfo << "\r\n";

		std::map<std::string, std::string> allHeaders = _headers.getAll();
		for (std::map<std::string, std::string>::const_iterator it = allHeaders.begin(); it != allHeaders.end(); ++it) {
			responseStream << it->first << ": " << it->second << "\r\n";
		}

		responseStream << "\r\n";
		responseStream << _body.str();
		return responseStream.str();
	}

	void Response::clear()
	{
		_protocol = "";
		_statusCode = 0;
		_statusInfo = "";
		_headers.clear();
		_body.clear();
	}
}
