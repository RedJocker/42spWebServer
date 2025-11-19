
// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   test_tcp.cpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/26 00:32:07 by maurodri          #+#    #+#             //
//   Updated: 2025/11/18 20:11:03 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //


#include "Application.hpp"
#include "ApplicationSpec.hpp"
#include <iostream>
#include <csignal>
#include <string>

http::Application setup_config_one(std::string &addressPort)
{
	config::ServerSpec serverSpec;
	serverSpec
		.setDocroot("./test/www")
		.setAddressPort(addressPort);

	config::VirtualServerSpec virtualServer1;
	virtualServer1
		.setUploadFolder("./test/www/uploads");
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
	virtualServer2.setDocroot("./test/www2")
		.setHostname("domain.com");
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

	config::ApplicationSpec appSpec;
	appSpec.addServer(serverSpec);

	return appSpec.toApplication();
}

void signalHandler(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << "Server shutdown required: "<< std::endl;
		conn::EventLoop::shouldExit = true;
	}
}

int run_server_config_one(std::string addressPort)
{
	http::Application app  = setup_config_one(addressPort);
	return app.run();
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return 42;
	signal(SIGINT, &signalHandler);
	std::string testToRun = std::string(argv[1]);
	if ("config_one" == testToRun) {
		if (argc != 3)
			return 42;
		return run_server_config_one(std::string(argv[2]));
	}
	return 69;
}
