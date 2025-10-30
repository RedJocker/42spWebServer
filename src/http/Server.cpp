/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:05:25 by vcarrara          #+#    #+#             */
//   Updated: 2025/10/30 00:08:44 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Monitor.hpp"
#include "pathUtils.hpp"
#include "devUtil.hpp"
#include "RequestPath.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <cerrno>
#include <cstdlib>
#include <cstring>

namespace http
{
	const std::string Server::DEFAULT_DOCROOT = "./www";
	Server::Server(const std::string &hostname,
				   const std::string &docroot,
				   unsigned short port)
		: conn::TcpServer(port), hostname(hostname), docroot(docroot)
	{
		this->docroot = docroot;
		while (!this->docroot.empty()
			   && this->docroot[this->docroot.size() - 1] == '/')
		{
			// keeping docroot without ending /
			// must ensure path has leading / on RequestPath
			this->docroot.erase(this->docroot.end() - 1);
		}
		this->docroot = this->docroot.empty() ?
			DEFAULT_DOCROOT : this->docroot;
	}

	Server::Server(const Server &other) : conn::TcpServer(other.port)
	{
		*this = other;
	}

	Server &Server::operator=(const Server &other)
	{
		if (this != &other)
		{
			this->hostname = other.hostname;
			this->docroot = other.docroot;
			this->port = other.port;
		}
		return *this;
	}

	Server::~Server(void) {}

	const std::string &Server::getHostname() const
	{
		return this->hostname;
	}

	const std::string &Server::getDocroot() const
	{
		return this->docroot;
	}

	void Server::addRoute(Route *route)
	{
		this->routes.insert(route);
	}

	void Server::onFileRead(http::Client &client, const std::string &fileContent)
	{
		RequestPath &reqPath = client.getRequest().getPath();


		std::string mimeType = utils::guessMimeType(reqPath.getExtension());
		client.getResponse()
			.addHeader("Content-Type", mimeType)
			.setOk()
			.setBody(fileContent);
		client.setMessageToSend(client.getResponse().toString());
	}

	void Server::onFileWritten(http::Client &client)
	{
		client.getResponse()
			.setCreated();
		client.setMessageToSend(client.getResponse().toString());
	}

	void Server::onServerError(http::Client &client)
	{
		client.getResponse()
			.setInternalServerError();
		client.setMessageToSend(client.getResponse().toString());
	}

	void Server::handleGetFile(http::Client &client, conn::Monitor &monitor)
	{
		std::cout << "Server::handleGetFile " << std::endl;

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

	void Server::handleGetDirectory(
		http::Client &client, conn::Monitor &monitor)
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
			 << "<h1> Index of " << dirPath.substr(docroot.size())
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

	void Server::handlePost(http::Client &client, conn::Monitor &monitor)
	{
		const RequestPath &reqPath = client.getRequest().getPath();

		const std::string &filePath = reqPath.getFilePath();
		Response &response = client.getResponse();

		int fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
		{
			// TODO
			// we need to check reason failed to open
			// and give a response based on the reason
			std::cerr << "Failed to open file: " << filePath << std::endl;
			response.setInternalServerError();
			client.setMessageToSend(response.toString());
			return;
		}

		monitor.subscribeFileWrite(
			fd, client.getFd(), client.getRequest().getBody());
	}

	void Server::handleDelete(http::Client &client, conn::Monitor &monitor)
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


	void Server::serve(Client &client, conn::Monitor &monitor)
	{
		RequestPath &reqPath = client.getRequest().getPath();

		// Redirect if not '/' for directory listing
		if (reqPath.isDirectory() && reqPath.needsTrailingSlashRedirect()) {
			Response &response = client.getResponse();
			std::string location = reqPath.getPath() + "/";
			response.clear();
			response.setStatusCode(308);
			response.setStatusInfo("Permanent Redirect");
			response.addHeader("Location", location);
			response.addHeader("Content-Length", "0");
			client.setMessageToSend(response.toString());
			return;
		}
		const std::string &method = client.getRequest().getMethod();


		for (std::set<Route*>::iterator routeIt = this->routes.begin();
			 routeIt != this->routes.end();
			 ++routeIt)
		{
			if (!(*routeIt)) // test not NULL
				continue;
			Route &route = *(*routeIt);
			if (route.matches(reqPath, method))
			{
				// Maybe TODO: if order of resolution maters
				// then need to pass some comparator to std::set<Route*>
				// and resolve order in comparator or something else
				client.setRoute(&route);
				route.serve(client, monitor);
				return ;
			}
		}


		// TODO pass to class RouteStaticFile : public Route
		Response &response = client.getResponse();

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

		response.setNotFound();
		client.setMessageToSend(response.toString());
	}
}
