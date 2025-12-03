/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ApplicationSpec.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 16:24:53 by maurodri          #+#    #+#             */
/*   Updated: 2025/12/03 16:41:09 by bnespoli         ###   ########.fr       */
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

	ssize_t ApplicationSpec::isHttpDirective(const std::string &directive)
	{
		if (!utils::startsWith("http", directive))
		{
			return -1;
		}
		return utils::findOneOf(directive, 4, "{");
	}
	
	int ApplicationSpec::applicationConfigParse(char **av)
	{
		config::Scanner scanner;
		// TODO checar os casos de erros
		scanner.readContent(av[1]);
		std::string filecontent = scanner.getContent();
		ssize_t alreadyread = 0;
		while (alreadyread < static_cast<ssize_t>(filecontent.size()))
		{
			alreadyread = scanner.readDirective(filecontent, alreadyread, this->directives);
			if (alreadyread < 0)
			{
				std::cerr << "Error parsing directive" << std::endl;
				return -1;
			}
		}
		for (size_t i = 0; i < this->directives.size(); ++i)
		{
			ssize_t startBrace = this->isHttpDirective(this->directives[i]);
			std::cout << "Directive " << i << ": " << this->directives[i] << std::endl;
			if (startBrace > 4)
			{
				ServerSpec serverSpec;
				std::string insideBrace = this->directives[i].substr(startBrace + 1, 
					this->directives[i].size() - (startBrace + 2));  // TODO melhorar isso
				if (serverSpec.serverConfigParse(insideBrace, scanner) != 0)
				{
					std::cerr << "Error parsing server directive" << std::endl;
					return -1;
				}
				this->addServer(serverSpec);
			}
		}
		return 0;
	}
}
