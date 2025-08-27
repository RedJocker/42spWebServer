// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   EventLoop.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/26 17:06:06 by maurodri          #+#    #+#             //
//   Updated: 2025/08/26 22:30:28 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "EventLoop.hpp"
#include <stdexcept>
#include <unistd.h>
#include <iostream>

namespace conn
{

	EventLoop::EventLoop(): epollFd(epoll_create1(0)) // 0 == no flags
	{
	}

	EventLoop::EventLoop(const EventLoop &other) : epollFd(other.epollFd)
	{

	}

	EventLoop &EventLoop::operator=(const EventLoop &other)
	{
		if (this == &other)
			return *this;
		this->epollFd = other.epollFd;
		return *this;
	}

	EventLoop::~EventLoop()
	{
		close(this->epollFd);
	}

	bool EventLoop::isOk() const
	{
		return this->epollFd >= 0;
	}

	bool EventLoop::subscribeTcpServer(TcpServer *tcpServer)
	{
		if (!this->isOk())
		{
			return false;
		}

		struct epoll_event event;

		event.events = EPOLLIN; // subscribe for reads only, level triggered
		event.data.fd = tcpServer->getServerFd();

		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, event.data.fd, &event))
		{
			return false;
		}
		std::pair<MapServerIterator, bool> insertResult =
			servers.insert(std::make_pair(event.data.fd, tcpServer));
		return insertResult.second;
	}

	bool EventLoop::subscribeTcpClient(int fd)
	{
		if (!this->isOk())
		{
			return false;
		}

		struct epoll_event event;

		event.events = EPOLLIN|EPOLLOUT; // subscribe for reads and writes, level triggered
		event.data.fd = fd;

		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, event.data.fd, &event))
		{
			return false;
		}

		TcpClient *tcpClient = new TcpClient(fd);
		if (tcpClient != 0)
		{
			std::pair<MapClientIterator, bool> insertResult =
				clients.insert(std::make_pair(event.data.fd, tcpClient));
			return insertResult.second;
		}
		return false;
	}

	bool EventLoop::unsubscribeTcpClient(TcpClient *client , struct epoll_event *clientEvent)
	{
		int clientFd = clientEvent->data.fd;
		if (epoll_ctl(this->epollFd, EPOLL_CTL_DEL, clientFd, clientEvent) != 0)
			return false;
		if (this->clients.erase(clientFd) != 1)
			return false;
		delete client;
		return true;
	}

	bool EventLoop::loop()
	{
		if (!this->isOk())
		{
			return false;
		}

		struct epoll_event events[MAX_EVENTS];

		while (true)
		{
			// waiting for ready event from epoll
			int numReadyEvents =
				epoll_wait(this->epollFd, events, MAX_EVENTS, -1); // -1 without timeout

			// iterating through ready events
			for(int i = 0; i < numReadyEvents; i++) {
				// checking which fd to know what to do
				MapServerIterator serverIt = this->servers.find(events[i].data.fd);
				if (serverIt != this->servers.end())
				{
					TcpServer *server = serverIt->second;
					std::pair<int, std::string> connResult = server->connectToClient();
					int clientFd = connResult.first;
					if (connResult.first < 0)
					{
						std::string errorMessage = connResult.second;
						std::cout << errorMessage << std::endl;
						continue;
					}
					bool isSubscribed = this->subscribeTcpClient(clientFd);
					if (!isSubscribed)
					{
						std::string errorMessage = "failed to subscribe client";
						std::cout << errorMessage << std::endl;
						close(clientFd);
					}
					continue;
				}
				MapClientIterator clientIt = this->clients.find(events[i].data.fd);
				if (clientIt != this->clients.end())
				{
					TcpClient *client = clientIt->second;
					if (events[i].events & EPOLLIN) // fd has content to read
					{
						std::pair<ReadState, char *> readResult = client->readlineCrlf();
						if (readResult.first == BufferedReader::READING)
							continue; // has not finished reading, will finish a next epoll_wait
						else if (readResult.first == BufferedReader::DONE)
						{
							// has finished reading has message and client still alive
							// TODO if headers has Connection: close we should close

							char *messageToSend = readResult.second;
							std::cout << "done reading: "
									  << std::string(messageToSend)
									  << std::endl;
							client->setMessageToSend(std::string(messageToSend) + "\r\n");
							delete[] messageToSend;
						}
						else if (readResult.first == BufferedReader::NO_CONTENT)
						{
							// has finished reading has message and client has closed
							char *messageToSend = readResult.second;
							std::cout << "eof reading: "
									  << std::string(messageToSend)
									  << std::endl;
							client->setMessageToSend(std::string(messageToSend));

							delete[] messageToSend;
							unsubscribeTcpClient(client, events + i);
						}
						else if (readResult.first == BufferedReader::ERROR)
						{
							// reading failed for some reason
							//unsubscribe
							std::cout << "error reading: "
									  << std::string(readResult.second)
									  << std::endl;
							unsubscribeTcpClient(client, events + i);
							throw std::domain_error("TODO read ERROR");
						}
					}
					else if (events[i].events & EPOLLOUT) // fd is available to write
					{
						if(client->getWriterState() != BufferedWriter::WRITING)
							continue; // we don't have anything ready to write
						std::pair<WriteState, char*> flushResult = client->flushMessage();
						if (flushResult.first == BufferedWriter::DONE)
						{
							std::cout << "done writing response: "
									  << client->getMessageToSend()
									  << std::endl;
						}
						if (flushResult.first == BufferedWriter::ERROR)
						{
							throw std::domain_error("TODO write ERROR");
						}
					}
					else
					{
						throw std::domain_error("TODO else error");
					}
					continue;
				}
			}

		}
	}
}
