// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ServerSpec.cpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 10:44:24 by maurodri          #+#    #+#             //
//   Updated: 2025/11/17 22:17:11 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ServerSpec.hpp"
#include "VirtualServer.hpp"
#include "VirtualServerSpec.hpp"
#include "constants.hpp"
#include "Server.hpp"

namespace config {

	ServerSpec::ServerSpec():
		addressPort(DEFAULT_ADDRESS_PORT),
		docroot(DEFAULT_DOCROOT),
		maxSizeBody(MAX_SIZE_BODY_UNLIMITED),
		listDirectories(DEFAULT_LIST_DIRECTORIES),
		indexFile(""), // no default
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
				.setListDirectoriesIfUnset(this->listDirectories)
				.setIndexFileIfEmpty(this->indexFile)
				.addErrorPagesIfUnset(this->errorPages)
				.toVirtualServer();
			vservers.push_back(virtualServer);
		}
		http::Server *server = new http::Server(
			*this,
			vservers);
		return server;
	}
}
