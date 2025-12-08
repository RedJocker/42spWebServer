/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 13:22:28 by vcarrara          #+#    #+#             */
/*   Updated: 2025/12/04 17:10:29 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "devUtil.hpp"
#include <sstream>
#include <unistd.h>
#include "pathUtils.hpp"

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
	Response::~Response(void)
	{

	}

	Headers &Response::headers(void)
	{
		return this->_headers;
	}

	std::string Response::getHeader(const std::string &key) const {
		return _headers.getHeader(key);
	}

	int Response::getStatusCode(void) const {
		return this->_statusCode;
	}

	bool Response::setStatusCodeStr(const std::string &statusStr)
	{
		std::stringstream lineStream(statusStr);
		std::string statusCodeStr;
		std::string statusInfo;

		lineStream >> statusCodeStr;
		std::getline(lineStream, statusInfo);

		if (statusCodeStr.size() != 3)
			return false;
		int statusCode = 0;
		for (size_t i = 0; i < 3; ++i)
		{
			if (statusCodeStr.at(i) < '0' || statusCodeStr.at(i) > '9')
				return false;
			statusCode *= 10;
			statusCode += (statusStr.at(i) - '0');
		}
		this->_statusCode = statusCode;
		utils::trimInPlace(statusInfo);
		if (statusInfo != "")
			this->_statusInfo = statusInfo;
		else
			this->_statusInfo = this->statusInfoInfer(statusCode);
		return true;
	}

	Response &Response::setProtocol(const std::string &protocol) {
		_protocol = protocol;
		return *this;
	}
	Response &Response::setStatusCode(int code) {
		_statusCode = code;
		return *this;
	}
	Response &Response::setStatusInfo(const std::string &info) {
		_statusInfo = info;
		return *this;
	}

	Response &Response::addHeader(const std::string &key, const std::string &value) {
		_headers.addHeader(key, value);
		return *this;
	}

	Response &Response::addHeader(const std::string &key, const size_t value) {
		std::stringstream ss;
		ss << value;
		_headers.addHeader(key, ss.str());
		return *this;
	}

	Response &Response::setHeaderContentLength() {
		const size_t size = _body.size();
		this->addHeader("Content-Length", size);
		return *this;
	}

	Response &Response::setBody(const Body &body) {
		_body = body;
		return *this;
	}

	Response &Response::setBody(const std::string &body) {
		this->_body.setContent(body);
		this->setHeaderContentLength();
		return *this;
	}

	Response &Response::setOk()
	{
		return (*this)
			.setStatusCode(200)
			.setStatusInfo("Ok");
	}

	Response &Response::setForbidden()
	{
		this->clear();
		(*this)
			.setStatusCode(403)
			.setStatusInfo("Forbidden")
			.addHeader("Content-Length", "0");
		return *this;
	}

	Response &Response::setNotFound()
	{
		(*this)
			.setStatusCode(404)
			.setStatusInfo("Not Found")
			.addHeader("Content-Length", "0");
		return *this;
	}

	Response &Response::setImTeapot()
	{
		(*this)
			.setStatusCode(418)
			.setStatusInfo("I'm a teapot")
			.addHeader("Content-Length", "0");
		return *this;
	}

	Response &Response::setInternalServerError()
	{
		this->clear();
		(*this)
			.setStatusCode(500)
			.setStatusInfo("Internal Server Error")
			.addHeader("Content-Length", "0")
			.addHeader("Connection", "close");
		return *this;
	}

	Response &Response::setGatewayTimeout()
	{
		this->clear();
		(*this)
			.setStatusCode(504)
			.setStatusInfo("Gateway Timeout")
			.addHeader("Content-Length", "0")
			.addHeader("Connection", "close");
		return *this;
	}

	Response &Response::setBadRequest()
	{
		(*this)
			.setStatusCode(400)
			.setStatusInfo("Bad Request")
			// must close to ensure we don't keep buffered trash
			// that would be processed on another request
			.addHeader("Connection", "close")
			.addHeader("Content-Length", "0");
		return *this;
	}

	Response &Response::setCreated()
	{
		(*this)
			.setStatusCode(201)
			.setStatusInfo("Created")
			.addHeader("Content-Length", "0");
		return *this;
	}

	Response &Response::setEntityTooLarge()
	{
		this->clear();
		(*this)
			.setStatusCode(413)
			.setStatusInfo("Request Entity Too Large")
			.addHeader("Content-Length", "0")
			.addHeader("Connection", "close");
		return *this;
	}

	Response &Response::setMethodNotAllowed(const std::string &allowedAsString)
	{
		this->clear();
		(*this)
			.setStatusCode(405)
			.setStatusInfo("Method Not Allowed")
			.addHeader("Content-Length", "0")
			.addHeader("Allow", allowedAsString)
			;
		return *this;
	}

	std::string Response::toString(void) const {
		std::ostringstream responseStream;
		responseStream << _protocol << " " << _statusCode << " " << _statusInfo << "\r\n";

		responseStream << _headers.str();

		responseStream << "\r\n";
		responseStream << _body.str();
		return responseStream.str();
	}

	void Response::clear()
	{
		_protocol = "HTTP/1.1";
		_statusCode = 0;
		_statusInfo = "";
		_headers.clear();
		_body.clear();
	}

	const std::string Response::statusInfoInfer(int statusCode) const
	{
		switch(statusCode)
		{
		case 100: return "Continue";
		case 101: return "Switching Protocols";
		case 102: return "Processing Deprecated";
		case 103: return "Early Hints";
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";
		case 207: return "Multi-Status";
		case 208: return "Already Reported";
		case 226: return "IM Used";
		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 305: return "Use Proxy Deprecated";
		case 306: return "unused";
		case 307: return "Temporary Redirect";
		case 308: return "Permanent Redirect";
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Content Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 416: return "Range Not Satisfiable";
		case 417: return "Expectation Failed";
		case 418: return "I'm a teapot";
		case 421: return "Misdirected Request";
		case 422: return "Unprocessable Content";
		case 423: return "Locked";
		case 424: return "Failed Dependency";
		case 425: return "Too Early Experimental";
		case 426: return "Upgrade Required";
		case 428: return "Precondition Required";
		case 429: return "Too Many Requests";
		case 431: return "Request Header Fields Too Large";
		case 451: return "Unavailable For Legal Reasons";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version Not Supported";
		case 506: return "Variant Also Negotiates";
		case 507: return "Insufficient Storage";
		case 508: return "Loop Detected";
		case 510: return "Not Extended";
		case 511: return "Network Authentication Required";
		default: return "Unspecified";
		}
	}

	bool Response::isBodyEmpty(void) const
	{
		return this->_body.size() == 0;
	}
}
