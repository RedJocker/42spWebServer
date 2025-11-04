// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Route.cpp                                          :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 23:51:27 by maurodri          #+#    #+#             //
/*   Updated: 2025/11/04 14:00:28 by maurodri         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#include "Route.hpp"

namespace http {

	int Route::idGenerator = 0;

	Route::Route(void)
		: id(Route::idGenerator++), methodsAllowed(), pathSpecification("/")
	{
	}

	Route::Route(const std::string &pathSpecification)
		: id(Route::idGenerator++),
		  methodsAllowed(),
		  pathSpecification(pathSpecification)
	{
	}

	Route::Route(const Route &other)
		: id(other.id),
		  methodsAllowed(other.methodsAllowed),
		  pathSpecification(other.pathSpecification)
	{	
	}

	Route &Route::operator=(const Route &other)
	{
		if (this == &other)
			return *this;
		this->id = other.id;
		this->methodsAllowed = other.methodsAllowed;
		this->pathSpecification = other.pathSpecification;
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

	Route &Route::addMethod(const std::string &allowedMethod)
	{
		this->methodsAllowed.insert(allowedMethod);
		return *this;
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
}
