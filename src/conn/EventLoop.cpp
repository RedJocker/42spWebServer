// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   EventLoop.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/26 17:06:06 by maurodri          #+#    #+#             //
//   Updated: 2025/09/09 22:11:57 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "EventLoop.hpp"
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <cstring>

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

	void EventLoop::unsubscribeFd(EventList::iterator &eventIt)
	{
		close(eventIt->fd);
		eventIt = this->events.erase(eventIt) - 1;
	}

	void EventLoop::unsubscribeHttpClient(
		EventList::iterator &eventIt)
	{
		int clientFd = eventIt->fd;
		this->unsubscribeFd(eventIt);
		http::Client *client = this->clients.at(clientFd);
		this->clients.erase(clientFd);
		delete client;
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

	void EventLoop::handleClientRequest(
		http::Client *client, EventList::iterator &eventIt)
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
			unsubscribeHttpClient(eventIt);
		}
		else if (maybeCompleteRequest.state() == http::Request::READ_ERROR)
		{
			// reading failed for some reason
			//unsubscribe
			std::cout << "error reading: "
					  << std::endl;
			unsubscribeHttpClient(eventIt);
			throw std::domain_error("TODO read ERROR");
		}
	}

	void EventLoop::handleClientWriteResponse(
		http::Client *client, EventList::iterator &eventIt)
	{
		if(client->getWriterState() != BufferedWriter::WRITING)
			return; // we don't have anything ready to write
		std::pair<WriteState, char*> flushResult = client->flushMessage();
        bool shouldClose = client->getRequest()
			.getHeader("Connection") == "close";
		if (flushResult.first == BufferedWriter::DONE)
		{
			std::cout << "done writing response: "
					  << client->getResponse().toString()
					  << std::endl;
			shouldClose = client->getResponse().getHeader("Connection") == "close";
			client->clear();
		}
		else if (flushResult.first == BufferedWriter::ERROR)
		{
			throw std::domain_error("TODO write ERROR");
			client->clear();
		}
		if (shouldClose)
			unsubscribeHttpClient(eventIt);
	}

	bool EventLoop::loop()
	{
		while (true)
		{
			// waiting for ready event from epoll
			 // -1 without timeout
			int numReadyEvents = poll(this->events.data(), events.size(), -1);
			if (numReadyEvents < 0)
			{
				std::cout << "poll error: " << strerror(errno) << std::endl;
				continue;
			}
			else if (numReadyEvents == 0)
			{
				std::cout << "poll timeout: " << std::endl;
				continue;
			}
			for (EventList::iterator monitoredIt = this->events.begin();
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
						--numReadyEvents;
						continue;
					}
					MapClientIterator clientIt = this->clients.find(monitoredIt->fd);
					if (clientIt != this->clients.end())
					{
						http::Client *client = clientIt->second;
						this->handleClientRequest(client, monitoredIt);
						--numReadyEvents;
						continue;
					}
				} else if (monitoredIt->revents & POLLOUT)
				{ // fd is available for write
					MapClientIterator clientIt = this->clients.find(monitoredIt->fd);
					if (clientIt != this->clients.end())
					{
						http::Client *client = clientIt->second;
						this->handleClientWriteResponse(client, monitoredIt);
						--numReadyEvents;
						continue;
					}
				} else if (monitoredIt->revents & (POLLHUP | POLLERR))
				{ // close
					MapClientIterator clientIt = this->clients.find(monitoredIt->fd);
					if (clientIt != this->clients.end())
					{
						this->unsubscribeHttpClient(monitoredIt);
						--numReadyEvents;
						continue;
					}
				}
			}
		}
	}
}
