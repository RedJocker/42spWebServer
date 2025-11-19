// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Application.cpp                                    :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/15 16:08:52 by maurodri          #+#    #+#             //
//   Updated: 2025/11/17 22:27:18 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Application.hpp"
#include <iostream>

namespace http  {


	Application::Application(void)
		: servers(new std::vector<Server *>),
		  eventLoop(new conn::EventLoop)
	{

	}

	Application::Application(const std::vector<Server*> &servers)
		: servers(new std::vector<Server *>(servers)),
		  eventLoop(new conn::EventLoop)
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
		for (std::vector<Server*>::iterator serverIt = this->servers->begin();
			 serverIt != this->servers->end();
			 ++serverIt)
		{
			std::pair<int, std::string> maybeServerFd = (*serverIt)->createAndListen();
			if (maybeServerFd.first < 0)
			{
				std::cout << maybeServerFd.second << std::endl;
				this->shutdown();
				return 11;
			}

			this->eventLoop->subscribeHttpServer(*serverIt);
		}
		delete this->servers;
		bool resLoop = this->eventLoop->loop();

		delete this->eventLoop;
		if (!resLoop)
			return 22;
		return 0;
	}

	void Application::shutdown(void)
	{
		for (std::vector<Server*>::iterator serverIt = this->servers->begin();
			 serverIt != this->servers->end();
			 ++serverIt)
		{
			(*serverIt)->shutdown();
			delete (*serverIt);
		}
		delete this->servers;
	}
}
