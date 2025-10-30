/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 17:11:02 by maurodri          #+#    #+#             */
//   Updated: 2025/10/28 21:35:30 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"
#include <iostream>
#include <signal.h>


void signalHandler(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << "Server shutdown required: "<< std::endl;
		conn::EventLoop::shouldExit = true;
	}
}

int main(void)
{
	signal(SIGINT, &signalHandler);
	conn::EventLoop eventLoop;

	http::Server server("localhost", "./www", 8080);
	server.addCgiRoute("/todo.cgi");
	server.addCgiRoute("/42/todo.cgi");
	server.addCgiRoute("/42/loop.cgi");
	std::pair<int, std::string> maybeServerFd = server.createAndListen();
	if (maybeServerFd.first < 0)
	{
		std::cout << maybeServerFd.second << std::endl;
		return 11;
	}

	eventLoop.subscribeHttpServer(&server);

	bool resLoop = eventLoop.loop();

	if (!resLoop)
		return 22;
	return 0;
}
