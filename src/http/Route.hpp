// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Route.hpp                                          :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 21:17:55 by maurodri          #+#    #+#             //
/*   Updated: 2025/10/31 15:40:45 by maurodri         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#ifndef ROUTE_HPP
# define ROUTE_HPP

#include "Client.hpp"
#include "Operation.hpp"

namespace conn
{
	class Monitor;
}

namespace http
{
	class Client;

	class Route
	{
		static int idGenerator;
		int id;

	public:
		Route(void);
		Route(const Route &other);
		Route &operator=(const Route &other);
		virtual ~Route(void);
		virtual bool matches(const RequestPath &path, const std::string &method) const = 0;
		virtual void serve(http::Client &client,  conn::Monitor &monitor) const = 0;
		virtual void respond(http::Client &client, const Operation &operation) const = 0;

		void onServerError(http::Client &client) const;
		int getId(void) const;
	};
}


#endif
