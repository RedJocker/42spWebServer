// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/19 17:11:02 by maurodri          #+#    #+#             //
//   Updated: 2025/08/26 16:54:16 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "TcpServer.hpp"
#include <string>
#include <iostream>
#include <unistd.h>
#include "Request.hpp"
#include "TcpClient.hpp"

int loopClient(int clientFd) // will be changed when using epoll
{
	conn::TcpClient tcpClient(clientFd);
	std::pair<ReadState, char *> readResult;
	while (true) {
		readResult.first = BufferedReader::READING;
		while (readResult.first == BufferedReader::READING)
		{
			std::cout << "reading" << std::endl;
			readResult = tcpClient.readlineCrlf();
		}
		std::string messageFromClient;
		if (readResult.first == BufferedReader::DONE)
		{
			messageFromClient = std::string(readResult.second);
			std::cout << "done: " << messageFromClient << std::endl;
			delete[] readResult.second;
		}
		else if (readResult.first == BufferedReader::NO_CONTENT)
		{
			messageFromClient = std::string(readResult.second);
			std::cout << "eof: " << messageFromClient << std::endl;
			delete[] readResult.second;
			return 0;
		}
		else
		{
			std::cout << "error: " << std::string(readResult.second) << std::endl;
			return 22;
		}

		std::string messageToClient = messageFromClient;
		tcpClient.setMessageToSend(messageToClient + "\r\n");
		std::pair<WriteState, char *> writeResult;
		while (writeResult.first == BufferedWriter::WRITING)
		{
			std::cout << "writing" << std::endl;
			writeResult = tcpClient.flushMessage();
		}
		if (writeResult.first == BufferedWriter::ERROR)
		{
			std::cout << "error: " << std::string(writeResult.second) << std::endl;
			return 22;
		}
		else if (writeResult.first == BufferedWriter::DONE)
		{
			std::cout << "message sent"  << std::endl;
		}
	}
}

int main(void)
{
	http::Request request;

	conn::TcpServer server;
	std::pair<int, std::string> maybeServerFd = server.createAndListen(8080);
	if (maybeServerFd.first < 0)
	{
		std::cout << maybeServerFd.second << std::endl;
		return 11;
	}
	std::pair<int, std::string> maybeClientFd = server.connectToClient();
	if (maybeClientFd.first < 0)
	{
		std::cout << maybeClientFd.second << std::endl;
		return 11;
	}
	int clientFd = maybeClientFd.first;

	loopClient(clientFd);

	return 0;
}
