/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSpec.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 10:44:24 by maurodri          #+#    #+#             */
/*   Updated: 2025/12/09 18:13:31 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSpec.hpp"
#include "VirtualServer.hpp"
#include "VirtualServerSpec.hpp"
#include "constants.hpp"
#include "Server.hpp"
#include <iostream>
#include <sstream>

namespace config {

	ServerSpec::ServerSpec():
		addressPort(DEFAULT_ADDRESS_PORT),
		docroot(DEFAULT_DOCROOT),
		maxSizeBody(MAX_SIZE_BODY_UNLIMITED),
		listDirectories(DEFAULT_LIST_DIRECTORIES),
		indexFile(""), // no default
		cgiTimeout(DEFAULT_CGI_TIMEOUT),
		errorPages()
	{
	}

	ServerSpec::ServerSpec(const  ServerSpec &other)
	{
		*this = other;
	}

	ServerSpec &ServerSpec::operator=(const ServerSpec &other)
	{
		if (this == &other)
			return *this;
		this->addressPort = other.addressPort;
		this->docroot = other.docroot;
		this->maxSizeBody = other.maxSizeBody;
		this->listDirectories = other.listDirectories;
		this->indexFile = other.indexFile;
		this->cgiTimeout = other.cgiTimeout;
		this->errorPages = other.errorPages;
		this->virtualServers = other.virtualServers;
		return *this;
	}

	ServerSpec::~ServerSpec(void)
	{

	}

	const std::string &ServerSpec::getDocroot(void) const
	{
		return this->docroot;
	}

	const std::string &ServerSpec::getAddressPort(void) const
	{
		return this->addressPort;
	}

	const MapErrorPages &ServerSpec::getErrorPages(void) const
	{
		return this->errorPages;
	}

	ServerSpec &ServerSpec::setDocroot(const std::string &docroot)
	{
		this->docroot = docroot;
		return *this;
	}

	ServerSpec &ServerSpec::setAddressPort(const std::string &addressPort)
	{
		this->addressPort = addressPort;
		return *this;
	}

	ServerSpec &ServerSpec::setMaxSizeBody(const ssize_t maxSizeBody)
	{
		this->maxSizeBody = maxSizeBody;
		return *this;
	}

	ServerSpec &ServerSpec::setListDirectories(bool listDirectory)
	{
		this->listDirectories = listDirectory;
		return *this;
	}

	ServerSpec &ServerSpec::setIndexFile(const std::string &indexFile)
	{
		this->indexFile = indexFile;
		return *this;
	}

	ServerSpec &ServerSpec::setCgiTimeout(time_t cgiTimeout)
	{
		this->cgiTimeout = cgiTimeout;
		return *this;
	}

	ServerSpec &ServerSpec::addVirtualServer(VirtualServerSpec &virtualServer)
	{
		this->virtualServers.push_back(virtualServer);
		this->virtualServers
			.back()
			.setDocrootIfEmpty(this->getDocroot());
		return *this;
	}

	ServerSpec &ServerSpec::addErrorPage(
		unsigned short int status, const std::string &bodyPage)
	{
		this->errorPages.insert(std::make_pair(status, bodyPage));
		return *this;
	}

	http::Server *ServerSpec::toServer(void)
	{
		std::vector<http::VirtualServer *> vservers;

		for (std::vector<VirtualServerSpec>::iterator virtualServerIt
				 = this->virtualServers.begin();
			 virtualServerIt != this->virtualServers.end();
			 ++virtualServerIt)
		{
			http::VirtualServer *virtualServer = (*virtualServerIt)
				.setDocrootIfEmpty(this->docroot)
				.setMaxSizeBodyIfUnset(this->maxSizeBody)
				.setIndexFileIfEmpty(this->indexFile)
				.setCgiTimeoutIfUnset(this->cgiTimeout)
				.setListDirectoriesIfUnset(this->listDirectories)
				.addErrorPagesIfUnset(this->errorPages)
				.toVirtualServer();
			vservers.push_back(virtualServer);
		}
		http::Server *server = new http::Server(
			*this,
			vservers);
		return server;
	}

	int ServerSpec::interpretDirective(const std::string &directive, Scanner &scanner)
	{ // TODO handle errors
		ssize_t end;
		ssize_t prefixSize;

		if (utils::isDirectiveSimple("listen", directive, end, prefixSize))
		{
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			std::cout << "Setting addressPort to: " << value << std::endl;
			this->setAddressPort(value);
			return 0;
		}
		if (utils::isDirectiveSimple("root", directive, end, prefixSize))
		{
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			if(!utils::fileisDirectory(value))
			{
				std::cerr << "root is not Directory: " << value << std::endl;
				return -1;				
			}
			std::cout << "Setting docroot to: " << value << std::endl;
			this->setDocroot(value);
			return 0;
		}
		if (utils::isDirectiveSimple("index", directive, end, prefixSize))
		{
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			if(!utils::fileisRegular(value))
			{
				std::cerr << "index is not regular file: " << value << std::endl;
				return -1;
			}
			std::cout << "Setting index file to: " << value << std::endl;
			this->setIndexFile(value);
			return 0;
		}
		if (utils::isDirectiveSimple("client_max_body_size", directive, end, prefixSize))
		{
			std::string valueStr = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			std::stringstream ss(valueStr);
			ssize_t value;
			ss >> value;
			if(ss.fail())
			{
				std::cerr << "Failed to parse directive:" << directive << std::endl;
				return -1;
			}
			if(value < 0)
			{
				std::cerr << "Failed to parse: client_max_body_size < 0" << std::endl;
				return -1;
			}
			std::cout << "Setting Max body size to: " << value << std::endl;
			this->setMaxSizeBody(value);
			return 0;
		}
		if (utils::isDirectiveSimple("autoindex", directive, end, prefixSize))
		{
			std::string valueStr = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			if (!(valueStr == "on" || valueStr == "true" || valueStr == "off" || valueStr == "false")) 
			{
				std::cerr << "Invalid value for autoindex directive: " << valueStr << std::endl;
				return -1;
			}
			bool value = (valueStr == "on" || valueStr == "true");
			std::cout << "Setting list directories to: " << value << std::endl;
			this->setListDirectories(value);
			return 0;
		}
		if (utils::isDirectiveSimple("fastcgi_read_timeout", directive, end, prefixSize))
		{
			std::string valueStr = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			std::stringstream ss(valueStr);
			ssize_t value;
			ss >> value;
			if(ss.fail())
			{
				std::cerr << "Failed to parse directive:" << directive << std::endl;
				return -1;
			}
			if(value < 0)
			{
				std::cerr << "Failed to parse: fastcgi_read_timeout < 0" << std::endl;
				return -1;
			}
			std::cout << "Setting cgiTimeout to: " << value << std::endl;
			this->setCgiTimeout(value);
			return 0;
		}
		if (utils::isDirectiveSimple("error_page", directive, end, prefixSize))
		{
			std::string valueStr = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			std::stringstream ss(valueStr);
			std::string path;
			size_t status;
			ss >> status >> path;
			std::string content;
			if(ss.fail())
			{
				std::cerr << "Failed to parse directive:" << directive << std::endl;
				return -1;
			}
			if(status < 400 || status >= 600)
			{
				std::cerr << "Failed to parse: error_page not in range 400..599" << std::endl;
				return -1;
			}
			if(utils::readErrorPage(path, content) != 0)
			{
				std::cerr << "Error reading error page file: " << path << std::endl;
				return -1;
			}
			std::cout << "Setting error pages to: " << status << ", " 
				<< content << std::endl;
			this->addErrorPage(status, content);
			return 0;
		}
		std::string param;
		std::string innerDirectives;
		if (utils::isDirectiveCompound("server", directive, param, innerDirectives))
		{
			VirtualServerSpec vserverSpec;
			if (vserverSpec.virtualServerConfigParse(innerDirectives, scanner) != 0)
			{
				std::cerr << "Error parsing virtual server directive" << std::endl;
				return -1;
			}
			this->addVirtualServer(vserverSpec);
			return 0;
		}
		return 0;
	}

	int ServerSpec::serverConfigParse(
		const std::string &directive, Scanner &scanner)
	{
		ssize_t alreadyread = 0;
		while (alreadyread <static_cast<ssize_t>(directive.size()))
		{
			alreadyread = scanner
				.readDirective(directive, alreadyread, this->directives);
			if (alreadyread < 0)
			{
				std::cerr << "Error parsing directive" << std::endl;
				return -1;
			}
		}
		
		for (size_t i = 0; i < this->directives.size(); ++i)
		{
			std::cout << "Directive " << i << ": "
					  << this->directives[i] << std::endl;
			this->interpretDirective(this->directives[i], scanner);
		}
		return 0;
	}

	
}
