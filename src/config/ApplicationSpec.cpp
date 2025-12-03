/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ApplicationSpec.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 16:24:53 by maurodri          #+#    #+#             */
/*   Updated: 2025/12/03 14:16:57 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ApplicationSpec.hpp"
#include "Server.hpp"
#include "Application.hpp"
#include "Scanner.hpp"
#include <iostream>

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

	int ApplicationSpec::applicationConfigParse(char **av)
	{
		config::Scanner scanner;
		// TODO checar os casos de erros
		scanner.readContent(av[1]);
		std::string filecontent = scanner.getContent();
		size_t alreadyread = scanner.readDirective(filecontent, 0, this->directives);
		std::cout << "Directives read:" << this->directives.at(0) << std::endl;
		alreadyread = scanner.readDirective(filecontent, alreadyread, this->directives);
		std::cout << "Directives read:" << this->directives.at(1) << std::endl;
		return 0;
	}
}
