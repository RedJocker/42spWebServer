/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 17:11:02 by maurodri          #+#    #+#             */
//   Updated: 2025/10/30 21:34:36 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"
#include <iostream>
#include <signal.h>
#include "RouteCgi.hpp"
#include "RouteStaticFile.hpp"

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

	http::RouteCgi cgiRoutes[3] = {
		http::RouteCgi("/todo.cgi"),
		http::RouteCgi("/42/todo.cgi"),
		http::RouteCgi("/42/loop.cgi")
	};
	for (size_t i = 0; i < sizeof(cgiRoutes) / sizeof(http::RouteCgi); ++i)
		server.addRoute(cgiRoutes + i);
	http::RouteStaticFile staticFileRoutes[1] = {
		http::RouteStaticFile("/"),
	};
	for (size_t i = 0; i < sizeof(staticFileRoutes) / sizeof(http::RouteStaticFile); ++i)
		server.addRoute(staticFileRoutes + i);

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
