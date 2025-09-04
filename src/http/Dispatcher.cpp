// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Dispatcher.cpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/09/03 17:41:30 by maurodri          #+#    #+#             //
//   Updated: 2025/09/03 20:17:59 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Dispatcher.hpp"

namespace http {

	Dispatcher::Dispatcher()
	{
	}

	Dispatcher::Dispatcher(const Dispatcher &other)
	{
		(void) other;
	}

	Dispatcher &Dispatcher::operator=(const Dispatcher &other)
	{
		if (this == &other)
			return *this;
	
		return *this;
	}

	Dispatcher::~Dispatcher()
	{
	}


	Response &Dispatcher::dispatch(http::Client &client,  http::MapServer &servers)
	{
		(void) servers;
		if (client.getRequest().getMethod() == "TRACE")
		{
			return client.getResponse()
				.setOk()
				.setBody(client.getRequest().toString());
		}
			
		return client.getResponse()
			.setNotFound();
	}

}
