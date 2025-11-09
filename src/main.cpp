/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 17:11:02 by maurodri          #+#    #+#             */
//   Updated: 2025/11/09 06:44:32 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"
#include "RouteCgi.hpp"
#include "RouteStaticFile.hpp"
#include "VirtualServer.hpp"
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

	// TODO: Improve instantiation of dependencies
	http::VirtualServer vservers[2] = {
		http::VirtualServer("localhost", "./www"),
		http::VirtualServer("domain.com", "./www2"),
	};


	http::RouteCgi cgiRoutes[1] = {
		http::RouteCgi("/**.cgi", "./www"),
	};

	for (size_t i = 0; i < sizeof(cgiRoutes) / sizeof(http::RouteCgi); ++i)
	{
		cgiRoutes[i]
			.addMethod("GET")
			.addMethod("POST");
		vservers[0].addRoute(cgiRoutes + i);
	}

	// TODO validate upload folder exists
	http::RouteStaticFile staticFileRoutes[1] = {
		http::RouteStaticFile("/**", "./www/uploads", "./www"),
	};

	for (size_t i = 0; i < sizeof(staticFileRoutes) / sizeof(http::RouteStaticFile); ++i)
	{
		staticFileRoutes[i]
			.addMethod("GET")
			.addMethod("POST")
			.addMethod("DELETE");
		vservers[0].addRoute(staticFileRoutes + i);
	}

	http::Server server("./www", 8080);
	http::RouteStaticFile otherStaticRoute("/**", "./www/uploads");
	otherStaticRoute
		.addMethod("GET");
	otherStaticRoute.setDocrootIfEmpty(vservers[1].getDocroot());
	vservers[1].addRoute(&otherStaticRoute);
	for (size_t i = 0; i < sizeof(vservers) / sizeof(http::VirtualServer); ++i)
	{
		server.addVirtualServer(vservers[i]);
	}
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
