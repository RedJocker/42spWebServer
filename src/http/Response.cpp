/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 13:22:28 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/15 14:20:46 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "devUtil.hpp"
#include <sstream>
#include <unistd.h>

namespace http {
	Response::Response(void)
	: _protocol("HTTP/1.1"),
	_statusCode(200),
	_statusInfo("OK"),
	_headers(),
	_body(),
	_fileFd(-1),
	_bytesSent(0),
	_isStreaming(false)
	{}
	Response::Response(const std::string &protocol, int statusCode, const std::string &statusInfo)
	: _protocol(protocol),
	_statusCode(statusCode),
	_statusInfo(statusInfo),
	_headers(),
	_body(),
	_fileFd(-1),
	_bytesSent(0),
	_isStreaming(false)
	{}
	Response::Response(const Response &other)
	: _statusCode(other._statusCode),
	_statusInfo(other._statusInfo),
	_headers(other._headers),
	_body(other._body),
	_fileFd(-1),
	_bytesSent(0),
	_isStreaming(false)
	{}
	Response &Response::operator=(const Response &other)
	{
		if (this != &other) {
			_protocol = other._protocol;
			_statusCode = other._statusCode;
			_statusInfo = other._statusInfo;
			_headers = other._headers;
			_body = other._body;

			closeFile();
			_fileFd = -1;
			_bytesSent = 0;
			_isStreaming = false;
		}
		return *this;
	}
	Response::~Response(void)
	{
		closeFile();
	}

	Headers &Response::headers(void)
	{
		return this->_headers;
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
		_isStreaming = false;
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
	    (*this)
	        .setStatusCode(500)
	        .setStatusInfo("Internal Server Error")
	        .addHeader("Content-Length", "0")
	        .addHeader("Connection", "Close");
		return *this;
	}

	Response &Response::setBadRequest()
	{
		(*this)
			.setStatusCode(400)
			.setStatusInfo("Bad Request")
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

	std::string Response::toString(void) const {
		std::ostringstream responseStream;
		responseStream << _protocol << " " << _statusCode << " " << _statusInfo << "\r\n";

		responseStream << _headers.str();

		responseStream << "\r\n";
		if (!_isStreaming) {
			responseStream << _body.str();
		}
		return responseStream.str();
	}

	void Response::clear()
	{
		_protocol = "HTTP/1.1";
		_statusCode = 0;
		_statusInfo = "";
		_headers.clear();
		_body.clear();
		closeFile();
	}

	void Response::setFileBody(int fd) {
		closeFile();
		_fileFd = fd;
		_bytesSent = 0;
		_isStreaming = true;
	}

	bool Response::isStreaming() const {
		return _isStreaming;
	}

	int Response::getFileFd() const {
		return _fileFd;
	}

	size_t Response::getBytesSent() const {
		return _bytesSent;
	}

	void Response::addBytesSent(size_t n) {
		_bytesSent += n;
	}

	void Response::closeFile() {
		if (_fileFd >= 0) {
			::close(_fileFd);
			_fileFd = -1;
		}
		_isStreaming = false;
		_bytesSent = 0;
	}
}
