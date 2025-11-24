// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Route.hpp                                          :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 21:17:55 by maurodri          #+#    #+#             //
//   Updated: 2025/11/24 17:58:10 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef ROUTE_HPP
# define ROUTE_HPP

# include "Client.hpp"
# include "Operation.hpp"
# include "RouteSpec.hpp"
# include "RequestPath.hpp"
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
		MapErrorPages errorPages;

	protected:
		std::string pathSpecification;
		std::string docroot;
		std::string indexFile;
		time_t cgiTimeout;
		bool listDirectories;
		std::pair<unsigned short int, std::string> redirection;

	public:
		Route(void);
		Route(const config::RouteSpec &routeSpec);
		Route(const Route &other);
		Route &operator=(const Route &other);
		virtual ~Route(void);
		virtual void serve(http::Client &client,  conn::Monitor &monitor) = 0;
		virtual void respond(http::Client &client, const Operation &operation) const = 0;
		// mutates path only if matches indexFile
		bool matches(RequestPath &path, const std::string &method) const;
		bool isMethodAllowed(const std::string &maybeMethodAllowed) const;
		void onServerError(http::Client &client) const;
		int getId(void) const;
		const std::string &getPathSpecification(void) const;
		const std::string &getDocroot(void) const;
		time_t getCgiTimeout(void) const;
		const MapErrorPages &getErrorPages(void) const;
		bool getListDirectories(void) const;
		bool hasRedirection(void) const;
		unsigned short int getRedirectionStatusCode(void) const;
		const std::string &getRedirectionPath(void) const;
	};
}

#endif
