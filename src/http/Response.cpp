/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 11:22:02 by vcarrara          #+#    #+#             */
/*   Updated: 2025/09/02 12:29:50 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sstream>

namespace http {
	Response::Response(void) {
		this->_protocol = "HTTP/1.1";
		this->_statusCode = 200;
		this->_statusInfo = "OK";
	}
	Response::Response(const std::string &protocol, int statusCode, const std::string &statusInfo) {
		this->_protocol = protocol;
		this->_statusCode = statusCode;
		this->_statusInfo = statusInfo;
	}
	Response::Response(const Response &other) {
		*this = other;
	}
	Response &Response::operator=(const Response &other) {
		if (this != &other) {
			this->_protocol = other._protocol;
			this->_statusCode = other._statusCode;
			this->_statusInfo = other._statusInfo;
			this->_headers = other._headers;
			this->_body = other._body;
		}
		return *this;
	}
	Response::~Response(void) {}

	// Static method to create a 404 Not Found response
	Response Response::NotFound()
	{
		Response r("HTTP/1.1", 404, "Not Found");

		std::string body;
		body += "<!DOCTYPE html>\r\n";
		body += "<html>\r\n";
		body += "<head><meta charset=\"utf-8\"><title>404 Not Found</title></head>\r\n";
		body += "<body>\r\n";
		body += "<h1>404 Not Found</h1>\r\n";
		body += "<p>The requested resource was not found on this server.</p>\r\n";
		body += "</body>\r\n";
		body += "</html>\r\n";

		r.setHeader("Content-Type", "text/html; charset=utf-8");
		r.setBody(body);

		return r;
	}

	// Setters
	void Response::setProtocol(const std::string &protocol) {
		this->_protocol = protocol;
	}
	void Response::setStatusCode(int code) {
		this->_statusCode = code;
	}
	void Response::setStatusInfo(const std::string &info) {
		this->_statusInfo = info;
	}
	void Response::setHeader(const std::string &key, const std::string &value) {
		this->_headers.setHeader(key, value);
	}
	void Response::setBody(const std::string &body) {
		this->_body.setContent(body);
	}

	// Getters
	std::string Response::getProtocol(void) const {
		return this->_protocol;
	}
	int Response::getStatusCode(void) const {
		return this->_statusCode;
	}
	std::string Response::getStatusInfo(void) const {
		return this->_statusInfo;
	}
	const Headers &Response::getHeaders(void) const {
		return this->_headers;
	}
	const Body &Response::getBody(void) const {
		return this->_body;
	}

	// Generate raw HTTP response string
	std::string Response::toString(void) const
	{
		std::ostringstream oss;
		oss << this->_protocol << " " << this->_statusCode << " " << this->_statusInfo << "\r\n";

		std::string headersString = this->_headers.toString();

		if (!this->_headers.has("Content-Length"))
		{
			std::ostringstream len;
			len << this->_body.size();
			headersString += std::string("Content-Length: ") + len.str() + "\r\n";
		}

		oss << headersString;
		oss << "\r\n";
		oss << this->_body.str();

		return oss.str();
	}
}
