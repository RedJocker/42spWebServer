// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Application.cpp                                    :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/15 16:08:52 by maurodri          #+#    #+#             //
//   Updated: 2025/11/15 17:49:04 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Application.hpp"
#include <iostream>

namespace http  {


	Application::Application(void) : servers(), eventLoop()
	{

	}

	Application::Application(const std::vector<Server> &servers)
		: servers(servers), eventLoop()
	{

	}

	Application::Application(const Application &other)
	{
		*this = other;
	}

	Application &Application::operator=(const Application &other)
	{
		if (this == &other)
			return *this;
		this->servers = other.servers;
		this->eventLoop = other.eventLoop;
		return *this;
	}

	Application::~Application()
	{

	}

	int Application::run(void)
	{
		for (std::vector<Server>::iterator serverIt = this->servers.begin();
			 serverIt != this->servers.end();
			 ++serverIt)
		{
			std::pair<int, std::string> maybeServerFd = serverIt->createAndListen();
			if (maybeServerFd.first < 0)
			{
				std::cout << maybeServerFd.second << std::endl;
				this->shutdown();
				return 11;
			}

			this->eventLoop.subscribeHttpServer(&(*serverIt));
		}
		bool resLoop = this->eventLoop.loop();

		if (!resLoop)
			return 22;
		return 0;
	}

	void Application::shutdown(void)
	{
		for (std::vector<Server>::iterator serverIt = this->servers.begin();
			 serverIt != this->servers.end();
			 ++serverIt)
		{
			serverIt->shutdown();
		}
	}
}
