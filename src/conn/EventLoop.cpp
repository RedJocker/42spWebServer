// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   EventLoop.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/26 17:06:06 by maurodri          #+#    #+#             //
//   Updated: 2025/09/09 18:02:50 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "EventLoop.hpp"
#include <stdexcept>
#include <unistd.h>
#include <iostream>

namespace conn
{

	EventLoop::EventLoop()
	{
	}

	EventLoop::EventLoop(const EventLoop &other)
	{
		(void) other;
	}

	EventLoop &EventLoop::operator=(const EventLoop &other)
	{
		if (this == &other)
			return *this;
		return *this;
	}

	EventLoop::~EventLoop()
	{
	}


	bool EventLoop::subscribeTcpServer(TcpServer *tcpServer)
	{
		struct pollfd event;

		event.events = POLLIN; // subscribe for reads only
		event.fd = tcpServer->getServerFd();

		this->events.push_back(event);
		std::pair<MapServerIterator, bool> insertResult =
			servers.insert(std::make_pair(event.fd, tcpServer));
		return insertResult.second;
	}

	bool EventLoop::subscribeHttpClient(int fd)
	{

		struct pollfd event;

		event.events = POLLIN|POLLOUT; // subscribe for reads and writes, level triggered
		event.fd = fd;

		http::Client *httpClient = new http::Client(fd);
		if (httpClient != 0)
		{
			this->events.push_back(event);
			std::pair<MapClientIterator, bool> insertResult =
				clients.insert(std::make_pair(event.fd, httpClient));
			return insertResult.second;
		}
		return false;
	}

	bool EventLoop::unsubscribeFd(int fd)
	{
		for (std::vector<struct pollfd>::iterator monitoredIt = this->events.begin();
			 monitoredIt != this->events.end();
			 ++monitoredIt)
		{
			if (monitoredIt->fd == fd)
			{
				this->events.erase(monitoredIt);
				return true;
			}
		}
		return false;
	}

	bool EventLoop::unsubscribeHttpClient(int clientFd)
	{
		if (!this->unsubscribeFd(clientFd))
			return false;
		http::Client *client = this->clients.at(clientFd);
		if (this->clients.erase(clientFd) != 1)
			return false;
		delete client;
		return true;
	}

	void EventLoop::connectServerToClient(TcpServer *server)
	{
		std::pair<int, std::string> connResult = server->connectToClient();
		int clientFd = connResult.first;
		if (connResult.first < 0)
		{
			std::string errorMessage = connResult.second;
			std::cout << errorMessage << std::endl;
			return;
		}
		bool isSubscribed = this->subscribeHttpClient(clientFd);
		if (!isSubscribed)
		{
			std::string errorMessage = "failed to subscribe client";
			std::cout << errorMessage << std::endl;
			close(clientFd);
		}
	}

	void EventLoop::handleClientRequest(http::Client *client)
	{
		http::Request maybeCompleteRequest = client->readHttpRequest();
		if (maybeCompleteRequest.state() <= http::Request::READING_BODY)
			return; // has not finished reading, will finish a next epoll_wait
		else if (maybeCompleteRequest.state() == http::Request::READ_COMPLETE)
		{
			// has finished reading has message and client still alive
			// TODO if headers has Connection: close we should close
			http::Request request = maybeCompleteRequest;
			std::cout << "done reading: "
					  << request.getMethod() << " "
					  << request.getPath() << " "
					  << request.getProtocol() << " "
					  << std::endl;

			http::Response &response = dispatcher.dispatch(*client, this->servers);
			std::string messageToSend = response.toString();
			client->setMessageToSend(messageToSend);
		}
		else if (maybeCompleteRequest.state() == http::Request::READ_BAD_REQUEST)
		{
			// has finished reading has message and client still alive
			// TODO if headers has Connection: close we should close
			std::cout << "bad request reading: "
					  << std::endl;
			std::string messageToSend =
				client->getResponse()
				.setBadRequest()
				.toString();
			client->setMessageToSend(messageToSend);
		}
		else if (maybeCompleteRequest.state() == http::Request::READ_EOF)
		{
			// has finished reading has message but client has closed
			// TODO confirm that client is always closed on this case
			std::cout << "eof reading: " << std::endl;
			unsubscribeHttpClient(client->getFd());
		}
		else if (maybeCompleteRequest.state() == http::Request::READ_ERROR)
		{
			// reading failed for some reason
			//unsubscribe
			std::cout << "error reading: "
					  << std::endl;
			unsubscribeHttpClient(client->getFd());
			throw std::domain_error("TODO read ERROR");
		}
	}

	void EventLoop::handleClientWriteResponse(http::Client *client)
	{
		if(client->getWriterState() != BufferedWriter::WRITING)
			return; // we don't have anything ready to write
		std::pair<WriteState, char*> flushResult = client->flushMessage();
		if (flushResult.first == BufferedWriter::DONE)
		{
			std::cout << "done writing response: "
					  << client->getResponse().toString()
					  << std::endl;
			client->clear();
		}
		else if (flushResult.first == BufferedWriter::ERROR)
		{
			throw std::domain_error("TODO write ERROR");
			client->clear();
		}
	}

	bool EventLoop::loop()
	{
		while (true)
		{
			// waiting for ready event from epoll
			int numReadyEvents =
				poll(this->events.data(), events.size(), -1); // -1 without timeout

			for (std::vector<struct pollfd>::iterator monitoredIt = this->events.begin();
				 monitoredIt != this->events.end() && numReadyEvents > 0;
				 ++monitoredIt)
			{
				if (monitoredIt->revents & POLLIN)
				{ // fd is available for read

					MapServerIterator serverIt = this->servers.find(monitoredIt->fd);
					if (serverIt != this->servers.end())
					{
						TcpServer *server = serverIt->second;
						this->connectServerToClient(server);
					}
					MapClientIterator clientIt = this->clients.find(monitoredIt->fd);
					if (clientIt != this->clients.end())
					{
						http::Client *client = clientIt->second;
						this->handleClientRequest(client);
					}
					--numReadyEvents;

				} else if (monitoredIt->revents & POLLOUT)
				{ // fd is avalilable for write
					MapClientIterator clientIt = this->clients.find(monitoredIt->fd);
					if (clientIt != this->clients.end())
					{
						http::Client *client = clientIt->second;
						this->handleClientWriteResponse(client);
					}
					--numReadyEvents;

				} else if (monitoredIt->revents & (POLLHUP | POLLERR))
				{ // close
					MapClientIterator clientIt = this->clients.find(monitoredIt->fd);
					if (clientIt != this->clients.end())
					{
						this->unsubscribeHttpClient(monitoredIt->fd);
					}
					--numReadyEvents;
				}
			}

		}
	}
}
