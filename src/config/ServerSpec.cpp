// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ServerSpec.cpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 10:44:24 by maurodri          #+#    #+#             //
//   Updated: 2025/11/12 20:03:53 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ServerSpec.hpp"
#include "VirtualServer.hpp"
#include "VirtualServerSpec.hpp"
#include "constants.hpp"

namespace config {


	ServerSpec::ServerSpec():
		port(DEFAULT_PORT),
		docroot(DEFAULT_DOCROOT),
		maxSizeBody(MAX_SIZE_BODY_UNLIMITED),
		listDirectories(DEFAULT_LIST_DIRECTORIES)
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
		this->port = other.port;
		this->docroot = other.docroot;
		this->maxSizeBody = other.maxSizeBody;
		this->listDirectories = other.listDirectories;
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

	const unsigned short &ServerSpec::getPort(void) const
	{
		return this->port;
	}

	ServerSpec &ServerSpec::setDocroot(const std::string &docroot)
	{
		this->docroot = docroot;
		return *this;
	}

	ServerSpec &ServerSpec::setPort(const unsigned short &port)
	{
		this->port = port;
		return *this;
	}

	ServerSpec &ServerSpec::setMaxSizeBody(const ssize_t maxSizeBody)
	{
		this->maxSizeBody = maxSizeBody;
		return *this;
	}

	ServerSpec &ServerSpec::setListDirectories(bool listDirectory)
	{
		this->listDirectory = listDirectory;
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

	http::Server ServerSpec::toServer(void)
	{
		std::vector<http::VirtualServer> vservers;

		for (std::vector<VirtualServerSpec>::iterator virtualServerIt
				 = this->virtualServers.begin();
			 virtualServerIt != this->virtualServers.end();
			 ++virtualServerIt)
		{
			http::VirtualServer virtualServer = (*virtualServerIt)
				.setDocrootIfEmpty(this->docroot)
				.setMaxBodySizeIfUnset(this->maxBodySize)
				.setListDirectoriesIfUnset(this->listDirectories)
				.toVirtualServer();
			vservers.push_back(virtualServer);
		}
		http::Server server(
			this->docroot,
			this->port,
			vservers);
		return server;
	}
}
