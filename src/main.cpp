/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 17:11:02 by maurodri          #+#    #+#             */
//   Updated: 2025/11/12 17:37:29 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"
#include "VirtualServerSpec.hpp"
#include "ServerSpec.hpp"

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


	config::ServerSpec serverSpec;
	serverSpec
		.setDocroot("./www")
		.setPort(8080);

	config::VirtualServerSpec virtualServer1;
	virtualServer1
		.setUploadFolder("./www/uploads");
	{
		config::RouteSpec routeSpec[2];
		routeSpec[0]
			.setPathSpec("/**.cgi")
			.setCgiRoute()
			.addAllowedMethod("POST")
			.addAllowedMethod("GET");

		routeSpec[1]
			.setPathSpec("/**")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET")
			.addAllowedMethod("DELETE");

		for (size_t i = 0; i < sizeof(routeSpec) / sizeof(config::RouteSpec); ++i)
		{
			virtualServer1.addRoute(routeSpec[i]);
		}
	}
	serverSpec.addVirtualServer(virtualServer1);

	config::VirtualServerSpec virtualServer2;
	virtualServer2.setDocroot("./www2")
		.setHostname("domain.com"); // need to add to /etc/hosts '127.0.0.1 domain.com'
	{
		config::RouteSpec routeSpec[2];
		routeSpec[0]
			.setPathSpec("/**.cgi")
			.setCgiRoute()
			.addAllowedMethod("POST")
			.addAllowedMethod("GET");

		routeSpec[1]
			.setPathSpec("/**")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET")
			.addAllowedMethod("DELETE");

		for (size_t i = 0; i < sizeof(routeSpec) / sizeof(config::RouteSpec); ++i)
		{
			virtualServer2.addRoute(routeSpec[i]);
		}
	}
	serverSpec.addVirtualServer(virtualServer2);

	http::Server server = serverSpec.toServer();

	conn::EventLoop eventLoop;

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
