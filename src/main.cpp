/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 17:11:02 by maurodri          #+#    #+#             */
/*   Updated: 2025/11/26 20:16:38 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ApplicationSpec.hpp"
#include "Application.hpp"
#include "EventLoop.hpp"
#include "VirtualServerSpec.hpp"
#include "ServerSpec.hpp"
#include "Scanner.hpp"
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
		.setAddressPort("localhost:8080")
		.setListDirectories(true)
		.setMaxSizeBody(2);

	config::VirtualServerSpec virtualServer1;
	virtualServer1
		.setUploadFolder("./www/uploads");
	{
		config::RouteSpec routeSpec[3];
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

		routeSpec[2]
			.setPathSpec("/55/**")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET")
			.addAllowedMethod("DELETE")
			.setRedirection(307, "/42/");

		for (size_t i = 0; i < sizeof(routeSpec) / sizeof(config::RouteSpec); ++i)
		{
			virtualServer1.addRoute(routeSpec[i]);
		}
	}
	serverSpec.addVirtualServer(virtualServer1);

	config::VirtualServerSpec virtualServer2;
	virtualServer2.setDocroot("./www2")
		.setHostname("domain.com") // need to add to /etc/hosts '127.0.0.1 domain.com'
		;

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

	config::VirtualServerSpec virtualServer3;
	virtualServer3.setHostname("example.org") // need to add to /etc/hosts '127.0.0.1 example.org'
		.setRedirection(307, "http://localhost:8080")
		;
	{
		config::RouteSpec routeSpec[1];
		routeSpec[0]
			.setPathSpec("/**")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET")
			.addAllowedMethod("DELETE");
		for (size_t i = 0; i < sizeof(routeSpec) / sizeof(config::RouteSpec); ++i)
		{
			virtualServer3.addRoute(routeSpec[i]);
		}
	}
	serverSpec.addVirtualServer(virtualServer3);
	config::ApplicationSpec appSpec;
	appSpec.addServer(serverSpec);

	return appSpec.toApplication();
}

int applicationConfigParse(char **av)
{	
	config::Scanner scanner;
	return scanner.readContent(av[1]);
}

int main(int ac, char **av)
{
	signal(SIGINT, &signalHandler);

	if (ac > 1)
		return applicationConfigParse(av);
	http::Application app = applicationConfig();
	return app.run();
}
