/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 17:28:13 by maurodri          #+#    #+#             */
//   Updated: 2025/11/20 09:15:28 by maurodri         ###   ########.fr       //
/*                                                                            */
/******************************************************************************/

#include "VirtualServer.hpp"
#include "RouteCgi.hpp"
#include "RouteStaticFile.hpp"
#include "pathUtils.hpp"
#include <iostream>
#include <sstream>
#include <string>

namespace http
{
	bool VirtualServer::OrderRoutes::operator()(
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

	VirtualServer::VirtualServer(
		const config::VirtualServerSpec &spec,
		const std::vector<Route *> &routes)
		: hostname(spec.getHostname()),
		  docroot(spec.getDocroot()),
		  routes(routes.begin(), routes.end()),
		  errorPages(spec.getErrorPages())
	{
		this->docroot = utils::trimCopy(docroot);
		while (!this->docroot.empty()
			   && this->docroot[this->docroot.size() - 1] == '/')
		{
			// keeping docroot without ending /
			// must ensure path has leading / on RequestPath
			this->docroot.erase(this->docroot.end() - 1);
		}
	}

	VirtualServer::VirtualServer(const VirtualServer &other)
	{
		*this = other;
	}

	VirtualServer &VirtualServer::operator=(const VirtualServer &other)
	{
		if (this != &other)
		{
			this->hostname = other.hostname;
			this->docroot = other.docroot;
			this->routes = other.routes;
			this->errorPages = other.errorPages;
		}
		return *this;
	}

	VirtualServer::~VirtualServer(void) {}

	const std::string &VirtualServer::getDocroot() const
	{
		return this->docroot;
	}

	const MapErrorPages &VirtualServer::getErrorPages(void) const
	{
		return this->errorPages;
	}

	bool VirtualServer::matches(const std::string &hostname)
	{
		std::cout << this->hostname << "::matches: " << hostname
				  << (this->hostname == hostname ? " [YES]" : " [NO]")
				  << std::endl;
		return this->hostname == hostname;
	}

	void VirtualServer::serve(Client &client, conn::Monitor &monitor)
	{
		const std::string &method = client.getRequest().getMethod();

		for (std::set<Route*>::iterator routeIt = this->routes.begin();
			 routeIt != this->routes.end();
			 ++routeIt)
		{
			if (!(*routeIt)) // test not NULL
				continue;
			Route &route = *(*routeIt);
			RequestPath &reqPath = client.getRequest().getPath();
			if (route.matches(reqPath, method))
			{

				client.setRoute(&route);
				reqPath.analyzePath(route.getDocroot());
				// Redirect if not '/' for directory listing
				if (reqPath.isDirectory() && reqPath.needsTrailingSlashRedirect()) {
					Response &response = client.getResponse();
					std::string location = reqPath.getPath() + "/";
					response.clear();
					response.setStatusCode(308);
					response.setStatusInfo("Permanent Redirect");
					response.addHeader("Location", location);
					response.addHeader("Content-Length", "0");
					client.writeResponse();
					return;
				}
				route.serve(client, monitor);
				return ;
			}
		}
		Response &response = client.getResponse();
		response.clear();
		response.setNotFound();
		client.writeResponse();
	}

	const std::string &VirtualServer::getHostname(void) const
	{
		return this->hostname;
	}

	void VirtualServer::shutdown(void)
	{
		for (std::set<Route*>::iterator routeIt = this->routes.begin();
			 routeIt != this->routes.end();
			 ++routeIt)
		{
			Route *route = *routeIt;
			if (route)
				delete route;
		}
	}
}
