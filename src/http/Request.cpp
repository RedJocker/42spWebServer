/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 10:51:33 by vcarrara          #+#    #+#             */
//   Updated: 2025/11/04 20:17:09 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "BufferedReader.hpp"
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include "pathUtils.hpp"

namespace http
{
	Request::Request(void)
		: _method()
		, _pathRaw()
		, _protocol()
		, _headers()
		, _body()
		, _state(READING_REQUEST_LINE)
	{}

	Request::Request(const Request &other) {
		*this = other;
	}

	Request &Request::operator=(const Request &other) {
		if (this != &other) {
			this->_method = other._method;
			this->_pathRaw = other._pathRaw;
			this->_protocol = other._protocol;
			this->_headers = other._headers;
			this->_body = other._body;
			this->_state = other._state;
		}
		return *this;
	}

	Request::~Request(void) {}

	bool Request::parseRequestLine(const std::string &line) {
		std::istringstream lineStream(line);
		if (!(lineStream >> _method >> _pathRaw >> _protocol))
			return false;
		return true;
	}

	Request::ReadState Request::readRequestLine(BufferedReader &client)
	{
		std::pair<BufferedReader::ReadState, char*> result;
		result = client.readlineCrlf();

		switch(result.first) {
		case BufferedReader::READING: {
			return _state;
		}
		case BufferedReader::ERROR: {
			_state = READ_ERROR;
			return _state;
		}
		case BufferedReader::NO_CONTENT: {
			delete[] result.second;
			_state = READ_EOF;
			return _state;
		}
		case BufferedReader::DONE: {
			if (!parseRequestLine(std::string(result.second)))
			{
				_state = READ_BAD_REQUEST;
				delete[] result.second;
				return _state;
			}
			delete[] result.second;
			_state = READING_HEADERS;
			if (client.hasBufferedContent())
				return this->readHeaderLine(client);
			else
				return _state;
		}
		default: return _state;
		}
	}

	Request::ReadState Request::readHeaderLine(BufferedReader &reader)
	{
		std::pair<BufferedReader::ReadState, char*> result;

		result = reader.readlineCrlf();

		switch(result.first) {
		case BufferedReader::READING:
			return _state;
		case BufferedReader::ERROR: {
			_state = READ_ERROR;
			return _state;
		}
		case BufferedReader::NO_CONTENT: {
			delete[] result.second;
			_state = READ_EOF;
			return _state;
		}
		case BufferedReader::DONE:
		{
			std::string line(result.second);
			delete[] result.second;

			if (line.empty()) { // End of Headers
				std::string contentLength =
					_headers.getHeader("Content-Length");
				std::string transferEncoding = _headers.getHeader("Transfer-Encoding");
				std::string contentType = _headers.getHeader("Content-Type");

				if (!contentType.empty()) {
					std::string contentTypeLowercase = utils::lowercaseCopy(contentType);
					if (utils::startsWith("multipart/form-data", contentTypeLowercase)) {
						_multipartBoundary = ""; // clear previous boundaries
						size_t bpos = contentType.find("boundary=");
						if (bpos != std::string::npos) {
							std::string boundary = contentType.substr(bpos + 9);
							size_t semicolon = boundary.find(';');
							if (semicolon != std::string::npos) {
								boundary = boundary.substr(0, semicolon);
							}
							utils::trimInPlace(boundary);
							if (boundary.size() >= 2
								&& boundary[0] == '"'
								&& boundary[boundary.size() - 1] == '"')
								boundary = boundary.substr(1, boundary.size() - 2);
							if (!boundary.empty())
								_multipartBoundary = std::string("--") + boundary;
							else
								_state = READ_BAD_REQUEST; // multipart without boundary is badreq
						}
					}
				}

				if (!contentLength.empty())
					_state = READING_BODY;
				else if (transferEncoding == "chunked")
					_state = READING_CHUNKED_BODY;
				else
					_state = READ_COMPLETE; // No body to read
			}
			else if (!_headers.parseLine(line)) { // unexpected format
				_state = READ_BAD_REQUEST;
			}
			if (reader.hasBufferedContent()) {
				if (_state == READING_HEADERS)
					return readHeaderLine(reader);
 				else if (_state == READING_BODY)
					return readBody(reader);
			}
			return _state;
		}
		default:
			return _state;
		}
	}

	Request::ReadState Request::readBody(BufferedReader &reader)
	{
		std::pair<BufferedReader::ReadState, char*> result;
		std::string contentLength = _headers.getHeader("Content-Length");
		std::string contentType = _headers.getHeader("Content-Type");
		size_t expectedLength = 0;

		if (!contentLength.empty())
			expectedLength = std::strtoul(contentLength.c_str(), NULL, 10);

		result = reader.read(expectedLength);
		switch (result.first) {
		case BufferedReader::READING:
			return _state;
		case BufferedReader::ERROR:
			_state = READ_ERROR;
			return _state;
		case BufferedReader::NO_CONTENT:
			_state = READ_EOF;
			return _state;
		case BufferedReader::DONE:
		{
			std::string chunk(result.second, expectedLength);
			delete[] result.second;

			if (utils::startsWith("multipart/form-data", contentType))
			{
				_multipartBodyAccum.append(chunk);
				if (_multipartBodyAccum.size() >= expectedLength)
				{
					if (!parseMultipartBody()) {
						_state = READ_BAD_REQUEST;
						return _state;
					}
					_state = READ_COMPLETE;
				}
			}
			else
			{
				if (!_body.parse(chunk.c_str(), expectedLength))
					return _state;
				if (_body.size() >= expectedLength)
					_state = READ_COMPLETE;
			}
			return _state;
		}
		default:
			return _state;
		}
	}

	Request::ReadState Request::readChunkedBody(BufferedReader &reader) {
		while (true) {
			// Read chunk size line
			std::pair<BufferedReader::ReadState, char*> sizeLine = reader.readlineCrlf();
			if (sizeLine.first != BufferedReader::DONE)
				return _state;
			std::string sizeStr(sizeLine.second);
			delete[] sizeLine.second;

			// Hex to int
			size_t chunkSize = std::strtoul(sizeStr.c_str(), NULL, 16);
			if (chunkSize == 0) {
				_state = READ_COMPLETE;
				break; // end of chunks
			}

			// Read chunk data + \r\n
			std::pair<BufferedReader::ReadState, char*> chunkData = reader.read(chunkSize + 2);
			if (chunkData.first != BufferedReader::DONE)
				return _state;

			_body.parse(chunkData.second, chunkSize); // Ignores \r\n
			delete[] chunkData.second;
		}
		return _state;
	}

	Request::ReadState Request::readHttpRequest(BufferedReader &reader) {
		switch (_state) {
		case READING_REQUEST_LINE:
			return readRequestLine(reader);
		case READING_HEADERS:
			return readHeaderLine(reader);
		case READING_BODY:
			return readBody(reader);
		case READING_CHUNKED_BODY:
			return readChunkedBody(reader);
		case READ_BAD_REQUEST:
		case READ_EOF:
		case READ_ERROR:
		case READ_COMPLETE:
			return _state;
		default:
			return _state;
		}
	}

	const std::string &Request::getMethod() const { return _method; }
	const std::string &Request::getPathRaw() const { return _pathRaw; }
	const std::string &Request::getProtocol() const { return _protocol; }
	RequestPath &Request::getPath() { return _path; }
	std::string Request::getHeader(const std::string &key) const {
		return _headers.getHeader(key);
	}
	std::string Request::getBody() const { return _body.str(); }
	void Request::clear()
	{
		_method = "";
		_pathRaw = "";
		_protocol = "";
		_state = READING_REQUEST_LINE;
		_headers.clear();
		_body.clear();
	}

	std::string Request::toString() const
	{
		std::ostringstream requestStream;
		requestStream << _method << " " << _pathRaw << " " << _protocol << "\r\n";
		requestStream << _headers.str();
		requestStream << _body.str();
		return requestStream.str();
	}

	char **Request::envp(void) const
	{
		std::vector<std::string> envp;
		envpInit(envp);

		char **envp_on_heap = new char*[envp.size() + 1];
		if (!envp_on_heap)
			return reinterpret_cast<char **>(0);
		for (size_t i = 0; i < envp.size(); ++i) {
			size_t len = envp[i].size() + 1;
			envp_on_heap[i] = new char[len];
			if (!envp_on_heap[i])
			{
				for (size_t j = 0; j < i; ++j)
					delete[] envp_on_heap[j];
				return reinterpret_cast<char **>(0);
			}
			std::strncpy(envp_on_heap[i], envp[i].c_str(), len);
			envp_on_heap[i][len - 1] = '\0';
		}

		envp_on_heap[envp.size()] = 0;
		return envp_on_heap;
	}

	bool Request::hasMultipart(void) const
	{
		return !_multipartBoundary.empty();
	}

	const std::string &Request::getMultipartBoundary(void) const
	{
		return _multipartBoundary;
	}

	void Request::extractMultipartBoundary(void)
	{
		std::string contentType = _headers.getHeader("Content-Type");
		std::string key = "boundary=";
		std::string::size_type pos = contentType.find(key);
		if (pos != std::string::npos)
			_multipartBoundary = "--" + contentType.substr(pos + key.size());
	}

	bool Request::parseMultipartBody(void)
	{
		if (_multipartBoundary.empty())
		return false;

		std::string endBoundary = _multipartBoundary + "--";
		std::string::size_type pos = 0;
		std::string::size_type next = 0;

		while ((next = _multipartBodyAccum.find(_multipartBoundary, pos)) != std::string::npos) {
			std::string part = _multipartBodyAccum.substr(pos, next - pos);
			pos = next + _multipartBoundary.size();

			if (part.empty())
				continue;

			std::string::size_type headerEnd = part.find("\r\n\r\n");
			if (headerEnd == std::string::npos)
				continue;

			std::string headerBlock = part.substr(0, headerEnd);
			std::string body = part.substr(headerEnd + 4);

			// Extract filename
			std::string::size_type fnPos = headerBlock.find("filename=\"");
			if (fnPos != std::string::npos) {
				fnPos += 10;
				std::string::size_type fnEnd = headerBlock.find("\"", fnPos);
				std::string filename = headerBlock.substr(fnPos, fnEnd - fnPos);

				std::string filepath = "/tmp/" + filename;
				std::ofstream ofs(filepath.c_str(), std::ios::binary);
				if (ofs.is_open()) {
					ofs.write(body.data(), body.size());
					ofs.close();
					_uploadedFiles.push_back(filepath);
				}
			}
			else {
				_body.append(body.c_str(), body.size());
			}
	}

	// Trim trailing boundary
	if (_multipartBodyAccum.find(endBoundary) != std::string::npos)
		return true;
	return false;
	}

	void Request::envpInit(std::vector<std::string> &envp) const
	{
		const RequestPath &reqPath = _path;
		envp.clear();
		envp.push_back("GATEWAY_INTERFACE=CGI/1.1");
		envp.push_back("SERVER_SOFTWARE=webserv/1.0");
		envp.push_back("REQUEST_METHOD=" + _method);
		envp.push_back("SERVER_PROTOCOL=" + (_protocol.empty() ? "HTTP/1.1" : _protocol));
		envp.push_back("REDIRECT_STATUS=0");
		envp.push_back("SCRIPT_FILENAME=" + reqPath.getFilePath());

		if (this->_method == "POST") {
			std::stringstream contentLengthStream;
			contentLengthStream << _body.size();
			std::string contentLength = contentLengthStream.str();
			if (!contentLength.empty()) {
				envp.push_back(std::string("CONTENT_LENGTH=") + contentLength);
				std::string contentType;
				if (this->hasMultipart())
					contentType = "application/octet-stream";
				else {
					contentType = _headers.getHeader("Content-Type");
					if (contentType.empty())
						contentType = "application/x-www-form-urlencoded";
				}
				envp.push_back(std::string("CONTENT_TYPE=") + contentType);
			}
		}

		if (!reqPath.getQueryString().empty())
			envp.push_back("QUERY_STRING=" + reqPath.getQueryString());
	}
}
