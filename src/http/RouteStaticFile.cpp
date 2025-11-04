// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteStaticFile.cpp                                :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 22:34:26 by maurodri          #+#    #+#             //
/*   Updated: 2025/11/04 13:58:10 by maurodri         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#include "RouteStaticFile.hpp"
#include "Server.hpp"
#include "Monitor.hpp"
#include "pathUtils.hpp"
#include "RequestPath.hpp"
#include <sys/socket.h>
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>

namespace http {

	RouteStaticFile::RouteStaticFile()
	{
		//TODO
	}

	RouteStaticFile::RouteStaticFile(std::string pathSpecification)
		: Route(pathSpecification)
	{
		//TODO
	}

	RouteStaticFile::RouteStaticFile(
		const RouteStaticFile &other) : Route(other)
	{
		(void) other;
		//TODO
	}

	RouteStaticFile &RouteStaticFile::operator=(const RouteStaticFile &other)
	{
		if (this == &other)
			return *this;
		//TODO
		return *this;
	}

	RouteStaticFile::~RouteStaticFile()
	{
		//TODO
	}

	void RouteStaticFile::handleGetFile(http::Client &client, conn::Monitor &monitor) const
	{
		std::cout << "RouteStaticFile::handleGetFile " << std::endl;

		RequestPath &reqPath = client.getRequest().getPath();

		int fd = open(reqPath.getFilePath().c_str(), O_RDONLY);
		if (fd < 0) {
			// TODO
			// we need to check reason failed to open
			// and give a response based on the reason
			client.getResponse().setNotFound();
			client.setMessageToSend(client.getResponse().toString());
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
			client.setMessageToSend(response.toString());
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
			 << "<h1> Index of " << dirPath.substr(reqPath.getDocroot().size())
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
		client.setMessageToSend(response.toString());
	}

	void RouteStaticFile::handlePost(http::Client &client, conn::Monitor &monitor) const
	{
		const RequestPath &reqPath = client.getRequest().getPath();

		const std::string &filePath = reqPath.getFilePath();

		int fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
		{
			// TODO
			// we need to check reason failed to open
			// and give a response based on the reason
			std::cerr << "Failed to open file: " << filePath << std::endl;
			this->onServerError(client);
			return;
		}

		monitor.subscribeFileWrite(
			fd, client.getFd(), client.getRequest().getBody());
	}

	void RouteStaticFile::handleDelete(http::Client &client, conn::Monitor &monitor) const
	{
		// TODO
		// what to do if file is directory? delete folder ? error ?

		(void) monitor;

		const std::string &filePath =
			client.getRequest().getPath().getFilePath();
		Response &response = client.getResponse();

		int result = unlink(filePath.c_str());
		if (result == 0)
		{
			response.clear();
			response.setStatusCode(204);
			response.setStatusInfo("No Content");
		} else {
			// TODO
			// we need to check reason failed to unlink
			// and give a response based on the reason
			// it may be not found, permission issue, maybe something else
			response.setNotFound();
		}

		client.setMessageToSend(response.toString());
	}

	void RouteStaticFile::serve(http::Client &client, conn::Monitor &monitor) const
	{
		std::cout << "RouteStaticFile::serve: " <<  client.getFd() << std::endl;

		const RequestPath &reqPath = client.getRequest().getPath();
		const std::string &method = client.getRequest().getMethod();
		if (method == "GET" && (reqPath.isFile() || reqPath.isDirectory()))
		{
			if (reqPath.isDirectory())
				handleGetDirectory(client, monitor);
			else
				handleGetFile(client, monitor);

			return;
		}

		if (method == "POST")
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
		client.setMessageToSend(response.toString());
	}

	void RouteStaticFile::onFileRead(http::Client &client, const std::string &fileContent) const
	{
		RequestPath &reqPath = client.getRequest().getPath();


		std::string mimeType = utils::guessMimeType(reqPath.getExtension());
		client.getResponse()
			.addHeader("Content-Type", mimeType)
			.setOk()
			.setBody(fileContent);
		client.setMessageToSend(client.getResponse().toString());
	}

	void RouteStaticFile::onFileWritten(http::Client &client) const
	{
		client.getResponse()
			.setCreated();
		client.setMessageToSend(client.getResponse().toString());
	}

	void RouteStaticFile::respond(http::Client &client,  const Operation &operation) const
	{
		switch (operation.type) {
		case Operation::FILE_READ: return onFileRead(client, operation.content);
		case Operation::FILE_WRITE: return onFileWritten(client);
		default: throw std::domain_error("unexpected path");
		}
	}
}
