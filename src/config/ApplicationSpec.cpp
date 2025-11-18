// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ApplicationSpec.cpp                                :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/15 16:24:53 by maurodri          #+#    #+#             //
//   Updated: 2025/11/17 22:16:44 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ApplicationSpec.hpp"
#include "Server.hpp"
#include "Application.hpp"

namespace config
{

	ApplicationSpec::ApplicationSpec(void) : servers()
	{

	}

	ApplicationSpec::ApplicationSpec(const ApplicationSpec &other)
	{
		*this = other;
	}

	ApplicationSpec &ApplicationSpec::operator=(const ApplicationSpec &other)
	{
		if (this == &other)
			return *this;

		this->servers = other.servers;

		return *this;
	}

	ApplicationSpec::~ApplicationSpec(void)
	{

	}

	void ApplicationSpec::addServer(const ServerSpec &serverSpec)
	{
		this->servers.push_back(serverSpec);
	}

	http::Application ApplicationSpec::toApplication(void)
	{
		std::vector<http::Server*> _servers;
		for (std::vector<ServerSpec>::iterator serverIt = this->servers.begin();
			 serverIt != this->servers.end();
			 ++serverIt)
		{
			http::Server *server = serverIt->toServer();
			_servers.push_back(server);

		}

		return http::Application(_servers);
	}
}
