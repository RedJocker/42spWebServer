// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   test_tcp.cpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/20 22:12:21 by maurodri          #+#    #+#             //
//   Updated: 2025/08/20 22:17:44 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "TcpServer.hpp"
#include <string>
#include <iostream>
#include <unistd.h>

int main(void)
{
	conn::TcpServer server;
	std::pair<int, std::string> maybeServerFd = server.createAndListen(8080);
	if (maybeServerFd.first < 0)
	{
		std::cout << maybeServerFd.second << std::endl;
		return 11;
	}
	std::pair<int, std::string> maybeClientFd = server.connectToClient();
	if (maybeClientFd.first < 0)
	{
		std::cout << maybeClientFd.second << std::endl;
		return 11;
	}
	close(maybeClientFd.first);
	return 0;
}
