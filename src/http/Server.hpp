/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:00:29 by vcarrara          #+#    #+#             */
//   Updated: 2025/10/02 01:20:35 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include "TcpServer.hpp"
# include <string>
# include <vector>
# include "Client.hpp"

namespace conn
{
	class Monitor;
}

namespace http
{
    class Server : public conn::TcpServer
	{
		std::string hostname;
		std::string docroot;
		std::vector<std::string> cgiRoutes;

		void handleGetFile(http::Client &client, conn::Monitor &monitor);

		void handleGetDirectory(http::Client &client, conn::Monitor &monitor);
		void handlePost(http::Client &client, conn::Monitor &monitor);
		void handleDelete(http::Client &client, conn::Monitor &monitor);
		bool isCgiRequest(http::Client &client, conn::Monitor &monitor);
		void handleCgiRequest(http::Client &client, conn::Monitor &monitor);

	public:
		Server(const std::string &hostname,
			   const std::string &docroot,
			   unsigned short port);
		Server(const Server &other);
		Server &operator=(const Server &other);
		virtual ~Server(void);

		std::string getHostname() const
;
		std::string getDocroot() const;

		void addCgiRoute(const std::string &route);
		void onFileRead(http::Client &client, const std::string fileContent);
		void onFileWritten(http::Client &client);
		void onCgiResponse(http::Client &client, const std::string cgiResponse);
		void onServerError(http::Client &client);
		void serve(Client &client, conn::Monitor &monitor);
		const std::vector<std::string> &getCgiRoutes() const;

	};
}

#endif
