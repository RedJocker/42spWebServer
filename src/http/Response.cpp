/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 13:22:28 by vcarrara          #+#    #+#             */
//   Updated: 2025/09/11 04:22:37 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sstream>
#include <unistd.h>

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

	std::string Response::getHeader(const std::string &key) const {
		return _headers.getHeader(key);
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
		this->clear();
		return (*this)
			.setStatusCode(200)
			.setStatusInfo("Ok");
	}

	Response &Response::setNotFound()
	{
		this->clear();
		(*this)
			.setStatusCode(404)
			.setStatusInfo("Not Found")
			.addHeader("Content-length", "0");
		return *this;
	}

	Response &Response::setBadRequest()
	{
		this->clear();
		(*this)
			.setStatusCode(400)
			.setStatusInfo("Bad Request")
			.addHeader("Content-length", "0");
		return *this;
	}

	Response &Response::setCreated()
	{
		this->clear();
		(*this)
			.setStatusCode(201)
			.setStatusInfo("Created")
			.addHeader("Content-length", "0");
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
}
