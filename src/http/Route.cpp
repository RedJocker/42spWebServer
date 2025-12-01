// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Route.cpp                                          :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 23:51:27 by maurodri          #+#    #+#             //
//   Updated: 2025/12/01 17:18:59 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Route.hpp"
#include "RouteSpec.hpp"
#include <iostream>
#include "constants.hpp"
#include <sstream>

namespace http {

	int Route::idGenerator = 0;

	Route::Route(void)
		: id(Route::idGenerator++),
		  methodsAllowed(),
		  errorPages(),
		  pathSpecification("/"),
		  docroot(""),
		  indexFile(""),
		  cgiTimeout(CGI_TIMEOUT_NONE),
		  listDirectories(false),
		  maxSizeBody(MAX_SIZE_BODY_UNLIMITED),
		  redirection(0, "")
	{
	}

	Route::Route(const config::RouteSpec &routeSpec)
		: id(Route::idGenerator++),
		  methodsAllowed(
			  routeSpec.getAllowedMethods().begin(),
			  routeSpec.getAllowedMethods().end()),
		  errorPages(routeSpec.getErrorPages()),
		  pathSpecification(routeSpec.getPathSpec()),
		  docroot(routeSpec.getDocroot()),
		  indexFile(routeSpec.getIndexFile()),
		  cgiTimeout(routeSpec.getCgiTimeout()),
		  listDirectories(routeSpec.getListDirectories()),
		  maxSizeBody(routeSpec.getMaxSizeBody()),
		  redirection(routeSpec.getRedirection())
	{
	}

	Route::Route(const Route &other)
		: id(other.id),
		  methodsAllowed(other.methodsAllowed),
		  errorPages(other.errorPages),
		  pathSpecification(other.pathSpecification),
		  docroot(other.docroot),
		  indexFile(other.indexFile),
		  cgiTimeout(other.cgiTimeout),
		  listDirectories(other.listDirectories),
		  maxSizeBody(other.maxSizeBody),
		  redirection(other.redirection)
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
		this->indexFile = other.indexFile;
		this->cgiTimeout = other.cgiTimeout;
		this->listDirectories = other.listDirectories;
		this->redirection = other.redirection;
		this->maxSizeBody = other.maxSizeBody;
		return *this;
	}

	Route::~Route()
	{
	}

	bool Route::matches(RequestPath &path) const
	{
		bool matches = false;
		bool shouldTryIndex = path.isDirectory()
			&& (!this->indexFile.empty());
													 
		if (shouldTryIndex)
		{
			std::string indexPath = path.getOriginalPath()
				+ "/" + this->indexFile;
			RequestPath pathWithIndex;

			pathWithIndex
				.initRequestPath(indexPath);
			pathWithIndex
				.analyzePath(this->docroot);

			matches = pathWithIndex.isFile()
				&& pathWithIndex
					.matchesPathSpecification(this->pathSpecification);
			if (matches)
				path = pathWithIndex; // mutates path copying pathWithIndex
		}
		if (!matches)
		{
			matches = path.matchesPathSpecification(this->pathSpecification);
		}

		return matches;
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

	time_t Route::getCgiTimeout(void) const
	{
		return this->cgiTimeout;
	}

	const MapErrorPages &Route::getErrorPages(void) const
	{
		return this->errorPages;
	}

	const std::string &Route::getPathSpecification(void) const
	{
		return this->pathSpecification;
	}

	bool Route::getListDirectories(void) const
	{
		return this->listDirectories;
	}

	bool Route::hasRedirection(void) const
	{
		return this->redirection.first >= 300
			&& this->redirection.first < 400
			&& (!this->redirection.second.empty());
	}

	ssize_t Route::getMaxSizeBody(void) const
	{
		return this->maxSizeBody;
	}

	unsigned short int Route::getRedirectionStatusCode(void) const
	{
		return this->redirection.first;
	}

	const std::string &Route::getRedirectionPath(void) const
	{
		return this->redirection.second;
	}

	std::string Route::methodsAllowedAsHeaderLine(void) const
	{
		std::stringstream ss;
		size_t i = 0;
		size_t len = this->methodsAllowed.size();
		// assume HEAD is always a valid method
		// avoids issue with empty Allow header
		if (len == 0)
			return "HEAD";
		std::set<std::string>::iterator methodIt = this->methodsAllowed.begin();
		for (; i < len; ++i, ++methodIt)
		{
			ss << *methodIt << ", ";
		}
		ss << "HEAD";
		return ss.str();
	}
}
