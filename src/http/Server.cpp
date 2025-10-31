/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:05:25 by vcarrara          #+#    #+#             */
//   Updated: 2025/10/30 22:17:14 by maurodri         ###   ########.fr       //
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
	}
}
