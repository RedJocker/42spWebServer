// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   test_tcp.cpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/26 00:32:07 by maurodri          #+#    #+#             //
//   Updated: 2025/08/26 00:57:08 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //


#include "TcpServer.hpp"
#include "TcpClient.hpp"
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

int test_echo()
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
	conn::TcpClient tcpClient(clientFd);
	std::pair<ReadState, char *> readResult;
	while (readResult.first == BufferedReader::READING)
	{
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
		writeResult = tcpClient.flushMessage();
	}
	if (writeResult.first == BufferedWriter::ERROR)
	{
		std::cout << "error: " << std::string(writeResult.second) << std::endl;
	}
	else if (writeResult.first == BufferedWriter::DONE)
	{
		std::cout << "message sent"  << std::endl;
	}

	return 0;
}


int main(int argc, char *argv[])
{
	if (argc < 2)
		return 43;
	std::string testToRun = std::string(argv[1]);
	if ("connection" == testToRun)
		return test_connect();
	else if ("read" == testToRun)
		return test_read(::atoi(argv[2]));
	else if ("readlineCrlf" == testToRun)
		return test_readlineCrlf();
	else if ("echo" == testToRun)
		return test_echo();
	return 69;
}
