// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Route.cpp                                          :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 23:51:27 by maurodri          #+#    #+#             //
/*   Updated: 2025/10/31 15:41:11 by maurodri         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#include "Route.hpp"

namespace http {

	int Route::idGenerator = 0;
	
	Route::Route(void) : id(Route::idGenerator++)
	{
	}

	Route::Route(const Route &other) : id(other.id)
	{	
	}

	Route &Route::operator=(const Route &other)
	{
		if (this == &other)
			return *this;
		this->id = other.id;
		return *this;
	}

	Route::~Route()
	{
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
