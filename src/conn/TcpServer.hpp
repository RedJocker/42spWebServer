// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   TcpServer.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/20 19:24:46 by maurodri          #+#    #+#             //
//   Updated: 2025/08/20 21:06:31 by maurodri         ###   ########.fr       //
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
		int serverFd;
		int port;

	public:

		TcpServer();
		TcpServer(const TcpServer &other);
		virtual TcpServer &operator=(const TcpServer &other);
		virtual ~TcpServer();

		std::pair<int, std::string> createAndListen(int port);
		std::pair<int, std::string> connectToClient();
	};

}

#endif
