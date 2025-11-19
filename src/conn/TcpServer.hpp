// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   TcpServer.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/20 19:24:46 by maurodri          #+#    #+#             //
//   Updated: 2025/11/15 16:04:50 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef TCPSERVER_HPP
# define TCPSERVER_HPP

#include <string>
#include <netdb.h>

namespace conn
{
	class TcpServer
	{
	protected:
		int serverFd;
		std::string addressPort;

		TcpServer();
	public:
		TcpServer(const std::string &addressPort);

		TcpServer(const TcpServer &other);
		TcpServer &operator=(const TcpServer &other);
		virtual ~TcpServer();

		std::pair<int, std::string> createAndListen();
		std::pair<int, std::string> connectToClient();
		const std::string &getAddressPort() const;
		int getServerFd() const;
	};
}

#endif
