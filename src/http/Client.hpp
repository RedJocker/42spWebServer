/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:34:03 by maurodri          #+#    #+#             */
/*   Updated: 2025/11/05 20:29:57 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "TcpClient.hpp"
# include "Request.hpp"
# include "Response.hpp"

namespace http
{
	class Server;
	class Route;

	class Client : public conn::TcpClient
	{
		Server *server;
		Route *route;
		Request request;
		Response response;
		size_t pendingFileWrites;
		Client();

	public:

		Client(int clientFd, Server *server);
		Client(const Client &other);
		Client &operator=(const Client &other);
		virtual ~Client();

		Request &readHttpRequest();
		Request &getRequest();
		Response &getResponse();

		Server *getServer(void) const;
		Route *getRoute(void) const;
		void setRoute(Route *server);

		void clear(void);

		void pendingFileWritesIncrement(void);
		void pendingFileWritesDecrement(void);
		size_t getPendingFileWrites(void) const;
	};
}

#endif
