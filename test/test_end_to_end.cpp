
// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   test_tcp.cpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/26 00:32:07 by maurodri          #+#    #+#             //
//   Updated: 2025/11/21 08:35:40 by maurodri         ###   ########.fr       //
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

http::Application setup_config_invalid_cgi_bin(std::string &addressPort)
{
	config::ServerSpec serverSpec;
	serverSpec
		.setDocroot("./test/www")
		.setAddressPort(addressPort);

	config::VirtualServerSpec virtualServer1;
	{
		config::RouteSpec routeSpec[1];
		routeSpec[0]
			.setPathSpec("/**.cgi")
			.setCgiBinPath("a bin that does not exist")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET");

		for (size_t i = 0; i < sizeof(routeSpec) / sizeof(config::RouteSpec); ++i)
		{
			virtualServer1.addRoute(routeSpec[i]);
		}
	}
	serverSpec.addVirtualServer(virtualServer1);
	config::ApplicationSpec appSpec;
	appSpec.addServer(serverSpec);

	return appSpec.toApplication();
}

http::Application setup_config_error_pages(std::string &addressPort)
{
	config::ServerSpec serverSpec;
	serverSpec
		.setDocroot("./test/www")
		.setAddressPort(addressPort)
		.addErrorPage(400, "server 400")
		.addErrorPage(418, "server 418")
		.addErrorPage(404, "server 404")
		.addErrorPage(500, "server 500")
		.addErrorPage(504, "server 504");

	config::VirtualServerSpec virtualServer1;
	virtualServer1
		.addErrorPage(400, "virtualServer 400")
		.addErrorPage(402, "virtualServer 402")
		.addErrorPage(404, "virtualServer 404")
		.addErrorPage(500, "virtualServer 500")
		.addErrorPage(504, "virtualServer 504");
	{
		config::RouteSpec routeSpec[3];
		routeSpec[0]
			.setPathSpec("/**.cgi")
			.setCgiBinPath("/usr/bin/php-cgi")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET")
			.addErrorPage(400, "route /**.cgi 400")
			.addErrorPage(404, "route /**.cgi 404")
			.addErrorPage(500, "route /**.cgi 500")
			.addErrorPage(504, "route /**.cgi 504");
		routeSpec[1]
			.setPathSpec("/42/*.cgi")
			.setCgiBinPath("/usr/bin/php-cgi")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET")
			.addErrorPage(400, "route /42/*.cgi 400")
			.addErrorPage(404, "route /42/*.cgi 404")
			.addErrorPage(418, "route /42/*.cgi 418")
			.addErrorPage(500, "route /42/*.cgi 500")
			.addErrorPage(504, "route /42/*.cgi 504");
		routeSpec[2]
			.setPathSpec("/*")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET")
			.addAllowedMethod("DELETE")
			.addErrorPage(400, "route /* 400")
			.addErrorPage(404, "route /* 404")
			.addErrorPage(500, "route /* 500")
			.addErrorPage(504, "route /* 504");

		for (size_t i = 0; i < sizeof(routeSpec) / sizeof(config::RouteSpec); ++i)
		{
			virtualServer1.addRoute(routeSpec[i]);
		}
	}
	serverSpec.addVirtualServer(virtualServer1);

	config::VirtualServerSpec virtualServer2;
	virtualServer2
		.setHostname("domain.com")
		.addErrorPage(400, "virtualServer 400")
		.addErrorPage(402, "virtualServer 402")
		.addErrorPage(404, "virtualServer 404")
		.addErrorPage(418, "virtualServer 418")
		.addErrorPage(500, "virtualServer 500")
		.addErrorPage(504, "virtualServer 504");
	{
		config::RouteSpec routeSpec[3];
		routeSpec[0]
			.setPathSpec("/**.cgi")
			.setCgiBinPath("/usr/bin/php-cgi")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET")
			.addErrorPage(400, "route /**.cgi 400")
			.addErrorPage(404, "route /**.cgi 404")
			.addErrorPage(500, "route /**.cgi 500")
			.addErrorPage(504, "route /**.cgi 504");
		routeSpec[1]
			.setPathSpec("/42/*.cgi")
			.setCgiBinPath("/usr/bin/php-cgi")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET")
			.addErrorPage(400, "route /42/*.cgi 400")
			.addErrorPage(402, "route /42/*.cgi 402")
			.addErrorPage(404, "route /42/*.cgi 404")
			.addErrorPage(500, "route /42/*.cgi 500")
			.addErrorPage(504, "route /42/*.cgi 504");
		routeSpec[2]
			.setPathSpec("/*")
			.addAllowedMethod("POST")
			.addAllowedMethod("GET")
			.addAllowedMethod("DELETE")
			.addErrorPage(400, "route /* 400")
			.addErrorPage(404, "route /* 404")
			.addErrorPage(500, "route /* 500")
			.addErrorPage(504, "route /* 504");

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

int run_server_config_invalid_cgi_bin(std::string addressPort)
{
	http::Application app  = setup_config_invalid_cgi_bin(addressPort);
	return app.run();
}

int run_server_config_error_pages(std::string addressPort)
{
	http::Application app  = setup_config_error_pages(addressPort);
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
	if ("config_invalid_cgi_bin" == testToRun) {
		if (argc != 3)
			return 42;
		return run_server_config_invalid_cgi_bin(std::string(argv[2]));
	}
	if ("config_error_pages" == testToRun) {
		if (argc != 3)
			return 42;
		return run_server_config_error_pages(std::string(argv[2]));
	}

	return 69;
}
