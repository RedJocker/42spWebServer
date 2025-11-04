// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Route.hpp                                          :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 21:17:55 by maurodri          #+#    #+#             //
/*   Updated: 2025/11/03 16:25:31 by maurodri         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#ifndef ROUTE_HPP
# define ROUTE_HPP

# include "Client.hpp"
# include "Operation.hpp"
# include <set>

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
		std::set<std::string> methodsAllowed;

	protected:
		std::string pathSpecification;

	public:
		Route(void);
		Route(const std::string &pathSpecification);
		Route(const Route &other);
		Route &operator=(const Route &other);
		virtual ~Route(void);
		virtual void serve(http::Client &client,  conn::Monitor &monitor) const = 0;
		virtual void respond(http::Client &client, const Operation &operation) const = 0;
		bool matches(const RequestPath &path, const std::string &method) const;
		Route &addMethod(const std::string &methodAllowed);
		bool isMethodAllowed(const std::string &maybeMethodAllowed) const;
		void onServerError(http::Client &client) const;
		int getId(void) const;
	};
}


#endif
