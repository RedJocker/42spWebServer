/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteStaticFile.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 22:34:26 by maurodri          #+#    #+#             */
/*   Updated: 2025/12/04 19:30:39 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RouteStaticFile.hpp"
#include "RouteSpec.hpp"
#include "Server.hpp"
#include "Monitor.hpp"
#include "devUtil.hpp"
#include "pathUtils.hpp"
#include "RequestPath.hpp"
#include <sys/socket.h>
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <cerrno>
#include <cstring>

namespace http {

	RouteStaticFile::RouteStaticFile()
	{
	}

	RouteStaticFile::RouteStaticFile(const config::RouteSpec &routeSpec)
		: Route(routeSpec),
		  uploadFolder(routeSpec.getUploadFolder())
	{
	}

	RouteStaticFile::RouteStaticFile(const RouteStaticFile &other)
		: Route(other), uploadFolder(other.uploadFolder)
	{
		*this = other;
	}

	RouteStaticFile &RouteStaticFile::operator=(const RouteStaticFile &other)
	{
		if (this == &other)
			return *this;
		this->uploadFolder = other.uploadFolder;
		Route::operator=(other);
		return *this;
	}

	RouteStaticFile::~RouteStaticFile()
	{
	}

	void RouteStaticFile::handleGetFile(http::Client &client, conn::Monitor &monitor) const
	{
		std::cout << "RouteStaticFile::handleGetFile " << std::endl;

		RequestPath &reqPath = client.getRequest().getPath();
		std::cout << "filePath: " << reqPath.getFilePath() << std::endl;

		int fd = open(reqPath.getFilePath().c_str(), O_RDONLY);
		if (fd < 0) {
			int err = errno;
			std::string error = strerror(err);
			std::cout << "handleGetFile error " << err << " " << error << std::endl;
			std::cerr << "failed opening file "
					  << reqPath.getFilePath()
					  << ": "
					  << error <<std::endl;
			if (err == EACCES) {
				client.getResponse().setForbidden();
				client.writeResponse();
				return;
			}
			if (err == ENOENT) {
				client.getResponse().setNotFound();
				client.writeResponse();
				return;
			}
			std::cerr << "Unhandled error opening file: " << err << std::endl;
			this->onServerError(client);
			return;
		}
		std::cout << "clientFd = " << client.getFd() << std::endl;
		monitor.subscribeFileRead(fd, client.getFd());
	}

	void RouteStaticFile::handleGetDirectory(
		http::Client &client, conn::Monitor &monitor) const
	{
		(void) monitor;

		const RequestPath &reqPath = client.getRequest().getPath();
		Response &response = client.getResponse();

		const std::string &dirPath = reqPath.getFilePath();

		DIR *dir = opendir(dirPath.c_str());
		if (!dir)
		{
			response.setNotFound();
			client.writeResponse();
			return ;
		}

		struct dirent *entry;
		std::vector<std::string> entries;
		while ((entry = readdir(dir)) != NULL) {
			std::string name(entry->d_name);
			entries.push_back(name);
		}
		closedir(dir);

		std::stringstream html;
		html << "<html><body>"
			 << "<h1> Index of " << dirPath.substr(this->getDocroot().size())
			 << "</h1>"
			 << "<hr>"
			 << "<ul>";
		for (std::vector<std::string>::const_iterator it = entries.begin();
			 it != entries.end();
			 ++it)
		{
			html << "<li><a href=\"./" << *it << "\">" << *it << "</a></li>";
		}
		html << "</ul></body></html>";

		response.setOk()
			.addHeader("Content-Type", "text/html")
			.setBody(html.str());
		client.writeResponse();
	}

	// at this moment keep body from multipart as a regular body
	// write to file while on .handlePost using uploadFolder config
	bool RouteStaticFile::parseMultipartBody(
		const std::string &boundary, const std::string &body)
	{
		size_t pos = body.find(boundary) + boundary.size();
		size_t next = pos;

		while ((next = body.find(boundary, pos)) != std::string::npos) {
			std::string partStr = body.substr(pos, next - pos);
			pos = next + boundary.size();

			if (partStr.empty())
				return false;

			std::string::size_type headerEnd = partStr.find("\r\n\r\n");
			if (headerEnd == std::string::npos)
				return false;

			MultipartPart part;

			part.headers = partStr.substr(0, headerEnd);
			part.body = partStr.substr(headerEnd + 4);

			// Extract filename
			std::string::size_type fnPos = part.headers.find("filename=\"");
			if (fnPos == std::string::npos)
				return false;

			fnPos += 10;
			std::string::size_type fnEnd = part.headers.find("\"", fnPos);
			if (fnEnd == std::string::npos)
				return false;
			part.filename = part.headers.substr(fnPos, fnEnd - fnPos);
			size_t lastSlash = utils::findLastFromEnd('/', part.filename, 0);
			if (lastSlash < part.filename.size())
			{
				part.filename = part.filename.substr(lastSlash);
			}
			utils::trimInPlace(part.filename);
			if (part.filename.size() == 0)
				return false;

			_multipartParts.push_back(part);
		}

		std::string rest = body.substr(pos);
		if (rest != "--\r\n")
			return false;
		return true;
	}

	void RouteStaticFile::handlePost(http::Client &client, conn::Monitor &monitor)
	{
		Request &request = client.getRequest();
		RequestPath &reqPath = request.getPath();

		if (request.hasMultipart())
		{
			bool hasParsed = this->parseMultipartBody(
				request.getMultipartBoundary(), request.getBody());
			if (!hasParsed)
			{
				this->_multipartParts.clear();
				client.getResponse().setBadRequest();
				client.writeResponse();
				return ;
			}
		}
		else
		{   // not multipart using this just to keep filename
			MultipartPart part;
			part.filename = reqPath.getPath();
			part.body = request.getBody();
			_multipartParts.push_back(part);
		}
		// at this point _multipartParts has all files that should be written

		for (std::vector<MultipartPart>::iterator it = _multipartParts.begin();
			 it != _multipartParts.end();
			 ++it)
		{
			std::string path = this->uploadFolder + "/" + it->filename;
			std::cout << "creating file: " << path << std::endl;
			int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd < 0)
			{
				std::cout << "Failed to open file for writing: " << path << std::endl;
				int err = errno;
				std::cerr << "Errno: " << err << std::endl;
				if (err == EACCES) {
					std::cerr << "No permission to create file: " << path << std::endl;
					client.getResponse().setForbidden();
					client.writeResponse();
				}
				else if (err == ENOENT) {
					std::cout << "Directory does not exist for file: " << path << std::endl;
					client.getResponse().setNotFound();
					client.writeResponse();
				} else {
					std::cerr << "Failed to open file: " << path << std::endl;
					this->onServerError(client);
				}
				_multipartParts.clear();
				return;
			}
			monitor.subscribeFileWrite(fd, client.getFd(), it->body);
		}
		_multipartParts.clear();
	}

	void RouteStaticFile::handleDelete(http::Client &client, conn::Monitor &monitor) const
	{
		// TODO
		// If file is a dir (check with stat?), use rmdir if empty, 409/recursive? if not, instead unlink

		(void) monitor;

		const std::string &filePath =
			client.getRequest().getPath().getFilePath();
		Response &response = client.getResponse();

		int result = unlink(filePath.c_str());
		std::cout << "handleDelete unlink result: " << result << std::endl;
		if (result == 0)
		{
			response.clear();
			response.setStatusCode(204);
			response.setStatusInfo("No Content");
		} else {
			int err = errno;
			std::string error = strerror(err);
			std::cout << "handleGetFile error " << err << " " << error << std::endl;
			// TODO
			// we need to check reason failed to unlink
			// and give a response based on the reason
			// it may be not found, permission issue, maybe something else
			response.setNotFound();
		}

		client.writeResponse();
	}

	void RouteStaticFile::serve(http::Client &client, conn::Monitor &monitor)
	{
		std::cout << "RouteStaticFile::serve: " <<	client.getFd() << std::endl;

		const RequestPath &reqPath = client.getRequest().getPath();
		const std::string &method = client.getRequest().getMethod();
		if (method == "GET" && (reqPath.isFile() || reqPath.isDirectory()))
		{
			if (reqPath.isDirectory()
				&& this->indexFile.empty()
				&& this->getListDirectories())
			{
				handleGetDirectory(client, monitor);
				return;
			}
			else if (reqPath.isFile()) {
				handleGetFile(client, monitor);
				return;
			}

		}

		if (method == "POST" && !this->uploadFolder.empty())
		{
			handlePost(client, monitor);
			return ;
		}

		if (method == "DELETE" && (reqPath.isFile() || reqPath.isDirectory()))
		{
			handleDelete(client, monitor);
			return ;
		}

		Response &response = client.getResponse();
		response.setNotFound();
		client.writeResponse();
	}

	void RouteStaticFile::onFileRead(http::Client &client, const std::string &fileContent) const
	{
		RequestPath &reqPath = client.getRequest().getPath();


		std::string mimeType = utils::guessMimeType(reqPath.getExtension());
		client.getResponse()
			.addHeader("Content-Type", mimeType)
			.setOk()
			.setBody(fileContent);
		client.writeResponse();
	}

	void RouteStaticFile::onFileWritten(http::Client &client) const
	{
		client.getResponse()
			.setCreated();
		client.writeResponse();
	}

	void RouteStaticFile::respond(http::Client &client,	 const Operation &operation) const
	{
		switch (operation.type) {
		case Operation::FILE_READ: return onFileRead(client, operation.content);
		case Operation::FILE_WRITE: return onFileWritten(client);
		default: throw std::domain_error("unexpected path");
		}
	}
}
