/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 17:11:02 by maurodri          #+#    #+#             */
//   Updated: 2025/11/20 08:25:54 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "ApplicationSpec.hpp"
#include "Application.hpp"
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

http::Application applicationConfig(void)
{
	config::ServerSpec serverSpec;
	serverSpec
		.setDocroot("./www")
		.setAddressPort("localhost:8080");

	config::VirtualServerSpec virtualServer1;
	virtualServer1
		.setUploadFolder("./www/uploads");
	{
		config::RouteSpec routeSpec[2];
		routeSpec[0]
			.setPathSpec("/**.cgi")
			.setCgiBinPath("/usr/bin/php-cgi")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET");

		routeSpec[1]
			.setPathSpec("/*")
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
			.setCgiBinPath("/usr/bin/php-cgi")
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

int main(void)
{
	signal(SIGINT, &signalHandler);

	http::Application app = applicationConfig();
	return app.run();
}
