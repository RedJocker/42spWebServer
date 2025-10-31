// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Route.cpp                                          :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 23:51:27 by maurodri          #+#    #+#             //
//   Updated: 2025/10/30 21:50:05 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Route.hpp"

namespace http {

	Route::Route(void)
	{
		//TODO
	}

	Route::Route(const Route &other)
	{
		(void) other;
		//TODO
	}

	Route &Route::operator=(const Route &other)
	{
		if (this == &other)
			return *this;
		//TODO
		return *this;
	}

	Route::~Route()
	{
		//TODO
	}

	void Route::onServerError(http::Client &client) const
	{
		client.getResponse()
			.setInternalServerError();
		client.setMessageToSend(client.getResponse().toString());
	}
}
