// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Route.hpp                                          :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 21:17:55 by maurodri          #+#    #+#             //
//   Updated: 2025/10/30 00:10:44 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef ROUTE_HPP
# define ROUTE_HPP

#include "Client.hpp"

namespace conn
{
	class Monitor;
}

namespace http
{
	class Client;

	class Route
	{
	public:
		Route(void);
		Route(const Route &other);
		Route &operator=(const Route &other);
		virtual ~Route(void);
		virtual bool matches(const RequestPath &path, const std::string &method) const = 0;
		virtual void serve(http::Client &client,  conn::Monitor &monitor) const = 0;
		virtual void respond(http::Client &client, const std::string &operationContent) const = 0;
	};
}


#endif
