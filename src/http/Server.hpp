/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:00:29 by vcarrara          #+#    #+#             */
//   Updated: 2025/10/30 00:09:36 by maurodri         ###   ########.fr       //
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

		std::string hostname;
		std::string docroot;
		std::set<Route*> routes;

		void handleGetFile(http::Client &client, conn::Monitor &monitor);

		void handleGetDirectory(http::Client &client, conn::Monitor &monitor);
		void handlePost(http::Client &client, conn::Monitor &monitor);
		void handleDelete(http::Client &client, conn::Monitor &monitor);
		bool isCgiRequest(http::Client &client, conn::Monitor &monitor);
		void handleCgiRequest(http::Client &client, conn::Monitor &monitor);

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
		void onFileRead(http::Client &client, const std::string &fileContent);
		void onFileWritten(http::Client &client);
		void onCgiResponse(http::Client &client, const std::string cgiResponse);
		void onServerError(http::Client &client);
		void serve(Client &client, conn::Monitor &monitor);

	};
}

#endif
