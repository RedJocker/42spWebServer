/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:00:29 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/31 15:27:03 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include "TcpServer.hpp"
# include <string>
# include <set>
# include "Client.hpp"
# include "Route.hpp"

namespace http
{
    class Server : public conn::TcpServer
	{
		struct OrderRoutes
		{
			bool operator()(const Route *s1, const Route *s2) const;
		};

		std::string hostname;
		std::string docroot;
		std::set<Route*, OrderRoutes> routes;

	public:
		const static std::string DEFAULT_DOCROOT;

		Server(const std::string &hostname,
			   const std::string &docroot,
			   unsigned short port);
		Server(const Server &other);
		Server &operator=(const Server &other);
		virtual ~Server(void);

		const std::string &getHostname() const;
		const std::string &getDocroot() const;

		void addRoute(Route *route);
		void serve(Client &client, conn::Monitor &monitor);
	};
}

#endif
