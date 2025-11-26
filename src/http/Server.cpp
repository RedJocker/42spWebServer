/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:05:25 by vcarrara          #+#    #+#             */
//   Updated: 2025/11/24 16:07:01 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "RouteCgi.hpp"
#include "RouteStaticFile.hpp"
#include "constants.hpp"
#include "pathUtils.hpp"
#include <iostream>
#include <sstream>

namespace http
{
	Server::Server(void)
		: conn::TcpServer(DEFAULT_PORT),
		  docroot(DEFAULT_DOCROOT),
		  vservers(),
		  errorPages()
	{
	}

	Server::Server(
		const config::ServerSpec &spec,
		std::vector<VirtualServer*> &virtualServers)
		: conn::TcpServer(spec.getAddressPort()),
		  docroot(spec.getDocroot()),
		  vservers(virtualServers.begin(), virtualServers.end()),
		  errorPages(spec.getErrorPages())
	{
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

	Server::Server(const Server &other)
		: conn::TcpServer(other.addressPort),
		  docroot(other.docroot),
		  vservers(other.vservers),
		  errorPages(other.errorPages)
	{
	}

	Server &Server::operator=(const Server &other)
	{
		if (this != &other)
		{
			this->docroot = other.docroot;
			this->vservers = other.vservers;
			this->errorPages = other.errorPages;
			conn::TcpServer::operator=(other);
		}
		return *this;
	}

	Server::~Server(void) {}

	const std::string &Server::getDocroot() const
	{
		return this->docroot;
	}

	const MapErrorPages &Server::getErrorPages(void) const
	{
		return this->errorPages;
	}

	void Server::serve(Client &client, conn::Monitor &monitor)
	{
	    const std::string &host = client.getRequest().getHeader("Host");

	    if (host.empty())
	    { // use default virtual server
			std::cout << "using default virtual server" << std::endl;
			client.setVirtualServer(vservers.at(0));
		    vservers.at(0)->serve(client, monitor);
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
				client.writeResponse();

				return;
			}
		}
		else
		{
			hostname = host;
		}

		for (std::vector<VirtualServer*>::iterator vserverIt = vservers.begin();
			 vserverIt != this->vservers.end();
			 ++vserverIt)
		{

			if ((*vserverIt) && (*vserverIt)->matches(hostname))
			{
				client.setVirtualServer((*vserverIt));
				(*vserverIt)->serve(client, monitor);
				return ;
			}
		}
	    // use default virtual server
		std::cout << "using default virtual server" << std::endl;
		client.setVirtualServer(vservers.at(0));
		vservers.at(0)->serve(client, monitor);
	}

	void Server::shutdown(void)
	{
		for (std::vector<VirtualServer*>::iterator vserverIt = vservers.begin();
			 vserverIt != this->vservers.end();
			 ++vserverIt)
		{
			(*vserverIt)->shutdown();
			delete (*vserverIt);
		}
	};
}
