// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/19 17:11:02 by maurodri          #+#    #+#             //
//   Updated: 2025/08/24 21:21:02 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "TcpServer.hpp"
#include "BufferedReader.hpp"
#include <string>
#include <iostream>
#include <unistd.h>


int main(void)
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
	BufferedReader reader(clientFd);
	std::pair<ReadState, char *> readResult;

	while (readResult.first == BufferedReader::READING)
	{
		std::cout << "reading" << std::endl;
		readResult = reader.read(5);
	}
	if (readResult.first == BufferedReader::DONE)
	{
		std::cout << "done: " << std::string(readResult.second) << std::endl;
		delete[] readResult.second;
	}
	if (readResult.first == BufferedReader::NO_CONTENT)
	{
		std::cout << "eof: " << std::string(readResult.second) << std::endl;
		delete[] readResult.second;
	}
	else
	{
		std::cout << "error: " << std::endl;
	}

	close(maybeClientFd.first);
	return 69;
}
