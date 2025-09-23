/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:34:03 by maurodri          #+#    #+#             */
/*   Updated: 2025/09/23 13:23:09 by vcarrara         ###   ########.fr       */
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

	class Client : public conn::TcpClient
	{
		Server *server;
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

		Server *getServer() const;

		void clear(void);
	};
}

#endif
