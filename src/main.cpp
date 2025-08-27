// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/19 17:11:02 by maurodri          #+#    #+#             //
//   Updated: 2025/08/26 22:25:30 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "EventLoop.hpp"
#include <iostream>


int main(void)
{
	conn::EventLoop eventLoop;

	if (!eventLoop.isOk())
	{
		std::cout << "failed to create epoll" << std::endl;
		return 11;
	}
	conn::TcpServer server;
	std::pair<int, std::string> maybeServerFd = server.createAndListen(8080);
	if (maybeServerFd.first < 0)
	{
		std::cout << maybeServerFd.second << std::endl;
		return 11;
	}

	eventLoop.subscribeTcpServer(&server);
	bool resLoop = eventLoop.loop();

	if (!resLoop)
		return 22;
	return 0;
}
