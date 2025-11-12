// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   VirtualServerSpec.cpp                              :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 11:03:12 by maurodri          #+#    #+#             //
//   Updated: 2025/11/12 17:43:47 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "VirtualServerSpec.hpp"
#include "constants.hpp"

namespace config {


	VirtualServerSpec::VirtualServerSpec(void) :
		hostname(DEFAULT_HOSTNAME),
		docroot(""), // default on ServerSpec
		uploadFolder(DEFAULT_UPLOAD_FOLDER),
		routes()
	{

	}

	VirtualServerSpec::VirtualServerSpec(const  VirtualServerSpec &other)
	{
		*this = other;
	}

	VirtualServerSpec &VirtualServerSpec::operator=(const VirtualServerSpec &other)
	{
		if (this == &other)
			return *this;
		this->hostname = other.hostname;
		this->docroot = other.docroot;
		this->routes = other.routes;
		this->uploadFolder = other.uploadFolder;
		return *this;
	}

	VirtualServerSpec::~VirtualServerSpec(void)
	{

	}

	const std::string &VirtualServerSpec::getDocroot(void) const
	{
		return this->docroot;
	}

	VirtualServerSpec &VirtualServerSpec::setHostname(const std::string &hostname)
	{
		this->hostname = hostname;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setDocroot(const std::string &docroot)
	{
		this->docroot = docroot;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setDocrootIfEmpty(const std::string &docroot)
	{
		if (this->docroot.empty())
			this->docroot = docroot;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setUploadFolder(const std::string &uploadFolder)
	{
		this->uploadFolder = uploadFolder;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::addRoute(RouteSpec &route)
	{
		this->routes.push_back(route);
		this->routes.back().setDocrootIfEmpty(this->docroot);
		return *this;
	}

	http::VirtualServer VirtualServerSpec::toVirtualServer(void)
	{
		std::vector<http::Route*> _routes;

		for (std::vector<RouteSpec>::iterator routeIt
				 = this->routes.begin();
			 routeIt != this->routes.end();
			 ++routeIt)
		{
			http::Route *route = (*routeIt)
				.setDocrootIfEmpty(this->docroot)
				.setUploadFolderIfEmpty(this->uploadFolder)
				.toRoute();
			_routes.push_back(route);
		}
		http::VirtualServer virtualServer(
			this->hostname,
			this->docroot,
			_routes);
		return virtualServer;
	}
}
