// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   TcpServer.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/20 19:24:46 by maurodri          #+#    #+#             //
//   Updated: 2025/09/23 19:11:27 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef TCPSERVER_HPP
# define TCPSERVER_HPP

#include <utility>
#include <string>

namespace conn
{
	class TcpServer
	{
	protected:
		int serverFd;
		unsigned short port;

		TcpServer();
	public:
		TcpServer(unsigned short port);

		TcpServer(const TcpServer &other);
		TcpServer &operator=(const TcpServer &other);
		virtual ~TcpServer();

		std::pair<int, std::string> createAndListen();
		std::pair<int, std::string> connectToClient();
		unsigned short getPort() const;
		int getServerFd() const;
	};
}

#endif
