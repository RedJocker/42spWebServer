/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:05:25 by vcarrara          #+#    #+#             */
//   Updated: 2025/11/08 01:39:04 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "RouteCgi.hpp"
#include "RouteStaticFile.hpp"
#include "constants.hpp"
#include <iostream>
#include <sstream>

namespace http
{
	Server::Server(const std::string &docroot, unsigned short port)
		: conn::TcpServer(port), docroot(docroot)
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
			this->docroot = other.docroot;
			this->port = other.port;
		}
		return *this;
	}

	Server::~Server(void) {}

	const std::string &Server::getDocroot() const
	{
		return this->docroot;
	}

	void Server::addVirtualServer(VirtualServer &virtualServer)
    {
		this->vservers.push_back(virtualServer);
	}

	void Server::serve(Client &client, conn::Monitor &monitor)
	{
	    const std::string &host = client.getRequest().getHeader("Host");

	    if (host.empty())
	    { // use default virtual server
		    vservers.at(0).serve(client, monitor);
			return;
		}

		size_t sepI = host.find(':');
		unsigned short hostPort = 80;
		std::string hostname = "";
		if (sepI != std::string::npos)
		{
			std::istringstream ss(host);
			std::getline(ss, hostname, ':');
			ss >> hostPort;
			if (ss.fail() || !ss.eof())
			{
				client.getResponse().setBadRequest();
				client.setMessageToSend(client.getResponse().toString());
				return;
			}
		}
		else
		{
			hostname = host;
		}

		for (std::vector<VirtualServer>::iterator vserverIt = vservers.begin();
			 vserverIt != this->vservers.end();
			 ++vserverIt)
		{

			if (vserverIt->matches(hostname))
			{
				vserverIt->serve(client, monitor);
				return ;
			}
		}
	    // use default virtual server
		vservers.at(0).serve(client, monitor);
	}
}
