/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:05:25 by vcarrara          #+#    #+#             */
/*   Updated: 2025/11/04 14:03:44 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "RouteCgi.hpp"
#include "RouteStaticFile.hpp"

namespace http
{
	bool Server::OrderRoutes::operator()(
		http::Route const *routeA, http::Route const *routeB) const {
		// true if routeA has precedence over routeB
		if (routeA == routeB)
			return false;
		if (!routeA)
			return false;
		if (!routeB)
			return true;
		RouteCgi const *routeAAsCgi = dynamic_cast<RouteCgi const*>(routeA);
		RouteCgi const *routeBAsCgi = dynamic_cast<RouteCgi const*>(routeB);
		if (routeAAsCgi && routeBAsCgi)
			// giving precedence to route that was declared last
			return routeAAsCgi->getId() > routeBAsCgi->getId();
		RouteStaticFile const *routeAAsStatic = dynamic_cast<RouteStaticFile const*>(routeA);
		RouteStaticFile const *routeBAsStatic = dynamic_cast<RouteStaticFile const*>(routeB);
		if (routeAAsStatic && routeBAsStatic)
			// giving precedence to route that was declared last
			return routeAAsStatic->getId() > routeBAsStatic->getId();
		// we know they are not null and not same type give precedence to cgi
		if (routeAAsCgi)
			return true;
		return false;
	}

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
				client.setRoute(&route);
				route.serve(client, monitor);
				return ;
			}
		}

		Response &response = client.getResponse();
		response.clear();
		response.setNotFound();
		client.setMessageToSend(response.toString());
	}
}
