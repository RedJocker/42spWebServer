// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   TcpServer.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/20 19:36:49 by maurodri          #+#    #+#             //
//   Updated: 2025/08/26 17:26:15 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "TcpServer.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

namespace conn
{
	TcpServer::TcpServer()
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
		return *this;
	}

	TcpServer::~TcpServer()
	{
		if (this->serverFd >= 0)
		{
			std::cout << "Closing server with port "
					  << this->port
					  << " and fd "
					  << this->serverFd
					  << std::endl;
			close(this->serverFd);
		}
	}

	std::pair<int, std::string> TcpServer::createAndListen(int port)
	{
		this->serverFd = socket(AF_INET, SOCK_STREAM, 0);
		this->port = port;
		if (this->serverFd < 0)
		{
			std::string errorMessage = "Error creating server: " + std::string(strerror(errno));
			return std::make_pair(-1, errorMessage);
		}
		int reuseAddressValue = 1;
		int reuseResult =
			setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuseAddressValue, sizeof(int));
		if (reuseResult != 0)
		{
			std::string errorMessage =
				"Error setting server socket option: " + std::string(strerror(errno));
			return std::make_pair(-1, errorMessage);
		}
		sockaddr_in serverAddress;
    	serverAddress.sin_family = AF_INET; // use ipv4
    	serverAddress.sin_port = htons(port); // bind to port
    	serverAddress.sin_addr.s_addr = INADDR_ANY; // any ip address
		int bindResult = bind(
			this->serverFd, 
			reinterpret_cast<struct sockaddr*>(&serverAddress), 
			sizeof(serverAddress));
		if (bindResult != 0)
		{
			std::string errorMessage = "Error biding server: " + std::string(strerror(errno));
			return std::make_pair(-1, errorMessage);
		}
		int listenResult = listen(this->serverFd, 5);
		if (listenResult != 0)
		{
			std::string errorMessage =
				"Error server start listening: " + std::string(strerror(errno));
			return std::make_pair(-1, errorMessage);
		}
		
		std::cout << "Opening server listening on port "
				  << this->port
				  << " and fd "
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
}


