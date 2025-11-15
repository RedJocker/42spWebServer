// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   TcpServer.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/20 19:36:49 by maurodri          #+#    #+#             //
/*   Updated: 2025/11/14 22:20:38 by maurodri         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#include "TcpServer.hpp"
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include "constants.hpp"

namespace conn
{
	TcpServer::TcpServer()
		: addressPort(DEFAULT_ADDRESS_PORT)
	{
	}

	TcpServer::TcpServer(const std::string &addressPort)
		: addressPort(addressPort)
	{

	}

	TcpServer::TcpServer(const TcpServer &other)
	{
		*this = other;
	}

	TcpServer &TcpServer::operator=(const TcpServer &other)
	{
		if (this == &other)
			return *this;
		this->serverFd = other.serverFd;
		this->addressPort = other.addressPort;
		return *this;
	}

	TcpServer::~TcpServer()
	{
		if (this->serverFd >= 0)
		{
			std::cout << "Closing server with port "
				//<< this->port
					  << " and fd "
					  << this->serverFd
					  << std::endl;
		}
	}

	std::pair<int, std::string> TcpServer::createAndListen()
	{
		struct addrinfo hint;
		memset(&hint, 0, sizeof(struct addrinfo));
		hint.ai_family = AF_INET; // use ipv4
		hint.ai_socktype = SOCK_STREAM; // tcp type
		hint.ai_flags = AI_NUMERICSERV;
		hint.ai_protocol = 0;

		std::string address;
		std::string portStr;

		size_t sep = addressPort.find(':');
		if (sep == std::string::npos)
		{
			address = addressPort;
			portStr = DEFAULT_PORT;
		}
		else
		{
			address = addressPort.substr(0, sep);
			portStr = addressPort.substr(sep + 1);
		}
		if (address.empty())
		{
			address = DEFAULT_ADDRESS;
		}
		if (portStr.empty())
		{
			portStr = DEFAULT_PORT;
		}
		this->addressPort = address + ":" + portStr;
		struct addrinfo *resolvedAddress; // will be allocated
		int retAddrInfo = getaddrinfo(
			address.c_str(), portStr.c_str(), &hint, &resolvedAddress);
		if (retAddrInfo != 0)
		{
			return std::make_pair(
				-1, std::string(gai_strerror(retAddrInfo)));
		}
		this->serverFd = socket(
			resolvedAddress->ai_family,
			resolvedAddress->ai_socktype,
			resolvedAddress->ai_protocol);

		if (this->serverFd < 0)
		{
			freeaddrinfo(resolvedAddress);
			std::string errorMessage = "Error creating server "
				+ this->addressPort + " : " + std::string(strerror(errno));
			return std::make_pair(-1, errorMessage);
		}
		int reuseAddressValue = 1;
		int reuseResult =
			setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuseAddressValue, sizeof(int));
		if (reuseResult != 0)
		{
			close(this->serverFd);
			freeaddrinfo(resolvedAddress);
			std::string errorMessage =
				"Error setting server socket option: " + std::string(strerror(errno));
			return std::make_pair(-1, errorMessage);
		}

		int bindResult = bind(
			this->serverFd,
			resolvedAddress->ai_addr,
			resolvedAddress->ai_addrlen);
		if (bindResult != 0)
		{
			std::string errorMessage = "Error biding server "
				+ this->addressPort + " : "
				+ std::string(strerror(errno));

			freeaddrinfo(resolvedAddress);
			close(this->serverFd);
			return std::make_pair(-1, errorMessage);
		}
		freeaddrinfo(resolvedAddress);
		int listenResult = listen(this->serverFd, 5);
		if (listenResult != 0)
		{
			std::string errorMessage =
				"Error server start listening: " + std::string(strerror(errno));
			return std::make_pair(-1, errorMessage);
		}
		
		std::cout << "Opening server listening on "
			      << this->addressPort
				  << " with fd "
				  << this->serverFd
				  << std::endl;
		return std::make_pair(this->serverFd, "Ok");
	}

	std::pair<int, std::string> TcpServer::connectToClient()
	{
		if (this->serverFd < 0)
			return std::make_pair(-1, "Error trying to listen on invalid serverFd");
		int clientSocket = accept(this->serverFd, 0, 0);
		
		return std::make_pair(clientSocket, "Ok");
	}

	int TcpServer::getServerFd() const
	{
		return this->serverFd;
	}

	const std::string &TcpServer::getAddressPort() const {
		return this->addressPort;
	}
}


