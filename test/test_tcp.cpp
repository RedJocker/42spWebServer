// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   test_tcp.cpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/20 22:12:21 by maurodri          #+#    #+#             //
//   Updated: 2025/08/24 21:59:07 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "TcpServer.hpp"
#include "BufferedReader.hpp"
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>


int test_connect(void)
{
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
	close(maybeClientFd.first);
	return 0;
}

int test_read(size_t length)
{
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
	BufferedReader readerClient(clientFd);
	std::pair<ReadState, char *> readResult;
	while(readResult.first == BufferedReader::READING) {
		readResult = readerClient.read(length);
	}
	if (readResult.first == BufferedReader::DONE)
	{
		char *message = readResult.second;
		std::cout << "done: " << message << std::endl;
		delete[] message;
	}
	else if (readResult.first == BufferedReader::NO_CONTENT)
	{
		char *message = readResult.second;
		std::cout << "eof: " << message << std::endl;
		delete[] message;
	}
	else if (readResult.first == BufferedReader::ERROR)
	{
		char *message = readResult.second;
		std::cout << "error: " << message << std::endl;
	}
	
	close(maybeClientFd.first);
	return 0;
}

int test_readlineCrlf()
{
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
	BufferedReader readerClient(clientFd);
	std::pair<ReadState, char *> readResult;
	while(readResult.first == BufferedReader::READING) {
		readResult = readerClient.readlineCrlf();
	}
	if (readResult.first == BufferedReader::DONE)
	{
		char *message = readResult.second;
		std::cout << "done: " << message << std::endl;
		delete[] message;
	}
	else if (readResult.first == BufferedReader::NO_CONTENT)
	{
		char *message = readResult.second;
		std::cout << "eof: " << message << std::endl;
		delete[] message;
	}
	else if (readResult.first == BufferedReader::ERROR)
	{
		char *message = readResult.second;
		std::cout << "error: " << message << std::endl;
	}
	
	close(maybeClientFd.first);
	return 0;
}


int main(int argc, char *argv[])
{
	if (argc < 2)
		return 43;

	if ("connection" == std::string(argv[1]))
		return test_connect();
	else if ("read" == std::string(argv[1]))
		return test_read(::atoi(argv[2]));
	else if ("readlineCrlf" == std::string(argv[1]))
		return test_readlineCrlf();
	
	return 69;
}
