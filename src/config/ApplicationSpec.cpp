/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ApplicationSpec.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 16:24:53 by maurodri          #+#    #+#             */
//   Updated: 2025/12/10 12:27:11 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "ApplicationSpec.hpp"
#include "Server.hpp"
#include "Application.hpp"
#include "Scanner.hpp"
#include <iostream>
#include <sstream>

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

	std::string ApplicationSpec::toString(void) const{
		std::stringstream ss;
		
		ss << "ApplicationSpec with " << this->servers.size() << " servers.\n";
		for (size_t i = 0; i < this->servers.size(); ++i)
		{
			ss << "Server " << i << ":\n";
			ss << this->servers[i].toString() << "\n";
		}
		return ss.str();
	}
	
	int ApplicationSpec::applicationConfigParse(char **av)
	{
		config::Scanner scanner;
		if (scanner.readContent(av[1]) != 0)
			return -1;
		std::string filecontent = scanner.getContent();
		ssize_t alreadyread = 0;
		int counter = 0;
		while (alreadyread < static_cast<ssize_t>(filecontent.size()))
		{
			alreadyread = scanner.readDirective(filecontent, alreadyread, this->directives);
			if (alreadyread < 0)
			{
				std::cerr << "Error parsing directive" << std::endl;
				return -1;
			}
			std::cout << "Directive " << counter << ": " << this->directives[counter] << std::endl;
			++counter;
		}
		for (size_t i = 0; i < this->directives.size(); ++i)
		{
			
			std::string param;
			std::string innerDirectives;
			if (utils::isDirectiveCompound("http", this->directives[i], param, innerDirectives))
			{
				ServerSpec serverSpec;
				if (serverSpec.serverConfigParse(innerDirectives, scanner) != 0)
				{
					std::cerr << "Error parsing server directive" << std::endl;
					return -1;
				}
				std::cout << "adding serverSpec " << i << ": " << serverSpec.toString()
						  << std::endl
						  << std::endl; 
				this->addServer(serverSpec);
			}
		}
		return 0;
	}
}
