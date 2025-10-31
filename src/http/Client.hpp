/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:34:03 by maurodri          #+#    #+#             */
//   Updated: 2025/10/30 21:25:38 by maurodri         ###   ########.fr       //
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
	};
}

#endif
