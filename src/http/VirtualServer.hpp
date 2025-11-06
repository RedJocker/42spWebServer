/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 17:12:50 by maurodri          #+#    #+#             */
/*   Updated: 2025/11/06 18:49:20 by maurodri         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef VIRTUAL_SERVER
# define VIRTUAL_SERVER

# include "Route.hpp"
# include "Client.hpp"
namespace http {
	class VirtualServer
	{
		struct OrderRoutes
		{
			bool operator()(const Route *s1, const Route *s2) const;
		};

		std::string hostname;
		std::string docroot;
		std::set<Route*, OrderRoutes> routes;
	public:
		VirtualServer();
		VirtualServer(const std::string &hostname, const std::string &docroot);
		VirtualServer(const VirtualServer &other);

		VirtualServer& operator=(const VirtualServer &other);
		~VirtualServer();

		void addRoute(Route *route);
		void serve(Client &client, conn::Monitor &monitor);
		const std::string &getDocroot(void) const;
		void setDocroot(const std::string &docroot);
		bool matches(const std::string &hostname);
	};
}
#endif
