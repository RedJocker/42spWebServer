// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteSpec.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 11:29:09 by maurodri          #+#    #+#             //
//   Updated: 2025/11/10 01:27:52 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "RouteSpec.hpp"
#include "RouteCgi.hpp"
#include "RouteStaticFile.hpp"

namespace config
{
	RouteSpec::RouteSpec(void):
		isCgi(false),
		docroot(""),
		pathSpec("/**"),
		uploadFolder("./"),
		allowedMethods()
	{
	}

	RouteSpec::RouteSpec(const RouteSpec &other)
	{
		*this = other;
	}

	RouteSpec &RouteSpec::operator=(const RouteSpec &other)
	{
		if (this == &other)
			return *this;
		this->isCgi = other.isCgi;
		this->docroot = other.docroot;
		this->pathSpec = other.pathSpec;
		this->uploadFolder = other.uploadFolder;
		this->allowedMethods = other.allowedMethods;
		return *this;
	}

	RouteSpec::~RouteSpec(void)
	{

	}

	const std::string &RouteSpec::getDocroot(void) const
	{
		return this->docroot;
	}

	RouteSpec &RouteSpec::setDocroot(const std::string &docroot)
	{
		this->docroot = docroot;
		return *this;
	}

	RouteSpec &RouteSpec::setDocrootIfEmpty(const std::string &docroot)
	{
		if (this->docroot.empty())
			this->docroot = docroot;
		return *this;
	}

	const std::string &RouteSpec::getPathSpec(void) const
	{
		return this->pathSpec;
	}

	RouteSpec &RouteSpec::setPathSpec(const std::string &pathSpec)
	{
		this->pathSpec = pathSpec;
		return *this;
	}

	RouteSpec &RouteSpec::addAllowedMethod(const std::string &method)
	{
		this->allowedMethods.push_back(method);
		return *this;
	}

	bool RouteSpec::isCgiRoute(void) const
	{
		return this->isCgi;
	}

	RouteSpec &RouteSpec::setCgiRoute(void)
	{
		this->isCgi = true;
		return *this;
	}

	http::Route *RouteSpec::toRoute(void)
	{

		http::Route *route;
		if (this->isCgi)
		{
			route = new http::RouteCgi(this->pathSpec,
									   this->docroot,
									   this->allowedMethods);
		}
		else
		{
			route = new http::RouteStaticFile(this->pathSpec,
											  this->uploadFolder,
											  this->docroot,
											  this->allowedMethods);
		}
		return route;
	}
}
