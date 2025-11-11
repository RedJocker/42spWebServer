// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Route.cpp                                          :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 23:51:27 by maurodri          #+#    #+#             //
//   Updated: 2025/11/10 01:26:02 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Route.hpp"

namespace http {

	int Route::idGenerator = 0;

	Route::Route(void)
		: id(Route::idGenerator++), methodsAllowed(), pathSpecification("/"), docroot("")
	{
	}

	Route::Route(const std::string &pathSpecification,
				 const std::string &docroot,
				 const std::vector<std::string> methodsAllowed)
		: id(Route::idGenerator++),
		  methodsAllowed(methodsAllowed.begin(), methodsAllowed.end()),
		  pathSpecification(pathSpecification),
		  docroot(docroot)
	{
	}

	Route::Route(const Route &other)
		: id(other.id),
		  methodsAllowed(other.methodsAllowed),
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
		client.setMessageToSend(client.getResponse().toString());
	}

	int Route::getId(void) const
	{
		return this->id;
	}

	const std::string &Route::getDocroot(void) const
	{
		return this->docroot;
	}
}
