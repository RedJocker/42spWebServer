// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Route.cpp                                          :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 23:51:27 by maurodri          #+#    #+#             //
//   Updated: 2025/11/20 09:12:57 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Route.hpp"
#include "RouteSpec.hpp"

namespace http {

	int Route::idGenerator = 0;

	Route::Route(void)
		: id(Route::idGenerator++),
		  methodsAllowed(),
		  errorPages(),
		  pathSpecification("/"),
		  docroot("")
	{
	}

	Route::Route(const config::RouteSpec &routeSpec)
		: id(Route::idGenerator++),
		  methodsAllowed(
			  routeSpec.getAllowedMethods().begin(),
			  routeSpec.getAllowedMethods().end()),
		  errorPages(routeSpec.getErrorPages()),
		  pathSpecification(routeSpec.getPathSpec()),
		  docroot(routeSpec.getDocroot())
	{
	}

	Route::Route(const Route &other)
		: id(other.id),
		  methodsAllowed(other.methodsAllowed),
		  errorPages(other.errorPages),
		  pathSpecification(other.pathSpecification),
		  docroot(other.docroot)
	{
	}

	Route &Route::operator=(const Route &other)
	{
		if (this == &other)
			return *this;
		this->id = other.id;
		this->methodsAllowed = other.methodsAllowed;
		this->errorPages = other.errorPages;
		this->pathSpecification = other.pathSpecification;
		this->docroot = other.docroot;
		return *this;
	}

	Route::~Route()
	{
	}

	bool Route::matches(const RequestPath &path, const std::string &method) const
	{
		if (!this->isMethodAllowed(method))
			return false;
		return path.matchesPathSpecification(this->pathSpecification);
	}

	bool Route::isMethodAllowed(const std::string &maybeAllowedMethod) const
	{
		return this->methodsAllowed
			.find(maybeAllowedMethod) != methodsAllowed.end();
	}

	void Route::onServerError(http::Client &client) const
	{
		client.getResponse()
			.setInternalServerError();
		client.writeResponse();
	}

	int Route::getId(void) const
	{
		return this->id;
	}

	const std::string &Route::getDocroot(void) const
	{
		return this->docroot;
	}

	const MapErrorPages &Route::getErrorPages(void) const
	{
		return this->errorPages;
	}

	const std::string &Route::getPathSpecification(void) const
	{
		return this->pathSpecification;
	}
}
