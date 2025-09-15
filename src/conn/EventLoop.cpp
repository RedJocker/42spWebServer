// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   EventLoop.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/26 17:06:06 by maurodri          #+#    #+#             //
//   Updated: 2025/09/11 04:23:25 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "EventLoop.hpp"
#include "Monitor.hpp"
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cerrno>

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
		std::pair<MapServer::iterator, bool> insertResult =
			servers.insert(std::make_pair(event.fd, tcpServer));
		return insertResult.second;
	}

	bool EventLoop::subscribeHttpClient(int fd)
	{

		struct pollfd event;

		event.events = POLLIN|POLLOUT; // subscribe for reads and writes
		event.fd = fd;

		http::Client *httpClient = new http::Client(fd);
		if (httpClient != 0)
		{
			this->events.push_back(event);
			std::pair<MapClient::iterator, bool> insertResult =
				clients.insert(std::make_pair(event.fd, httpClient));
			return insertResult.second;
		}
		return false;
	}

	void EventLoop::unsubscribeFd(int fd)
	{
		close(fd);
		this->removeFds.insert(fd);
		// defer removing to approriate moment to avoid iterator invalidation
		// on EventLoop.loop
	}

	void EventLoop::unsubscribeHttpClient(
		ListEvents::iterator &eventIt)
	{
		std::cout << "unsubscribeHttpClient " <<  eventIt->fd << std::endl;
		int clientFd = eventIt->fd;

		for (MapFileReads::iterator it = fileReads.begin();
			 it != fileReads.end();
			 it++)
		{
			if (it->second->getFd() == clientFd)
			{
				close(it->first);
				unsubscribeFd(it->first);
				fileReads.erase(it);
				break;
			}
		}

		this->unsubscribeFd(eventIt->fd);
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
		} else {
			std::cout << "connected client " << clientFd << std::endl;
		}
	}

	void EventLoop::subscribeFileRead(int fileFd, int clientFd)
	{

		http::Client *client = this->clients.at(clientFd);
		if (client)
		{
			std::cout << "subscribeFileRead " << fileFd << std::endl;
			struct pollfd event;
			event.events = POLLIN; // subscribe for reads
			event.fd = fileFd;
			events.push_back(event);
			client->setOperationFd(fileFd);
			this->fileReads.insert(std::make_pair(fileFd, client));
		}
	}

	void EventLoop::subscribeFileWrite(int fileFd, int clientFd, std::string content)
	{
		http::Client *client = this->clients.at(clientFd);
		if (client)
		{
			std::cout << "subscribeFileWrite " << fileFd << std::endl;
			BufferedWriter writer(fileFd);
			writer.setMessage(content);
			std::pair<WriteState, char*> writeResult;
			// TODO make proper subscription on EventLoop
			while (writeResult.first == BufferedWriter::WRITING)
			{
				writeResult = writer.flushMessage();
			}
			if (writeResult.first == BufferedWriter::DONE)
			{
				std::cout << "file writing done done" << fileFd << std::endl;
				client->getResponse()
					.setCreated();
				client->setMessageToSend(client->getResponse().toString());
			} else
			{
				std::cout << "file writing error "
						  << writeResult.second
						  << " "
						  << fileFd << std::endl;
			}
		}
	}

	void EventLoop::handleFileReads(
		http::Client *client,  ListEvents::iterator &eventIt)
	{
		std::cout << "clientFd = " << client->getFd() << std::endl;
		std::cout << "handleFileReads" << std::endl;

		std::pair<BufferedReader::ReadState, char*> readResult
			= client->readAllOperationFd();

		if(readResult.first == BufferedReader::READING)
			return ;
		std::cout << "handleFileReads after reading" << std::endl;
		if (readResult.first == BufferedReader::NO_CONTENT)
		{
			std::string responseStr = std::string(readResult.second);
			delete [] readResult.second;

			client->getResponse()
				.setOk()
				.setBody(responseStr);

			client->setMessageToSend(client->getResponse().toString());
		} else
		{
			std::cout << "failed handleFileReads " << readResult.second << std::endl;
		}

		this->unsubscribeFd(eventIt->fd);
		this->fileReads.erase(this->fileReads.find(eventIt->fd));
	}

	void EventLoop::handleClientRequest(
		http::Client *client, ListEvents::iterator &eventIt)
	{
		http::Request maybeCompleteRequest = client->readHttpRequest();
		if (maybeCompleteRequest.state() <= http::Request::READING_BODY)
			return; // has not finished reading, will finish a next epoll_wait
		else if (maybeCompleteRequest.state() == http::Request::READ_COMPLETE)
		{
			// has finished reading has message and client still alive
			http::Request request = maybeCompleteRequest;
			std::cout << "done reading: "
					  << request.getMethod() << " "
					  << request.getPath() << " "
					  << request.getProtocol() << " "
					  << std::endl;

			dispatcher.dispatch(*client, *this);
		}
		else if (maybeCompleteRequest.state() == http::Request::READ_BAD_REQUEST)
		{
			// has finished reading has message and client still alive
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
		http::Client *client, ListEvents::iterator &eventIt)
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

			for (ListEvents::iterator monitoredIt = this->events.begin();
				 monitoredIt != this->events.end() && numReadyEvents > 0;
				 )
			{
				// remove unsubscrived from last iteration
				SetRemoveFd::iterator removeIt =
					this->removeFds.find(monitoredIt->fd);
				if (removeIt != this->removeFds.end())
				{
					std::cout << "removing  " << monitoredIt->fd << std::endl;
					monitoredIt = this->events.erase(monitoredIt);
					this->removeFds.erase(removeIt);
					continue;
				}
				//std::cout << "monitoredFd  " << monitoredIt->fd << std::endl;
				if (monitoredIt->revents & (POLLHUP | POLLERR))
				{ // close
					std::cout << "close " << monitoredIt->fd << std::endl;
					MapClient::iterator clientIt = this->clients.find(monitoredIt->fd);
					if (clientIt != this->clients.end())
					{
						this->unsubscribeHttpClient(monitoredIt);
						--numReadyEvents;
						++monitoredIt;
						continue;
					}
				}
				else if (monitoredIt->revents & POLLIN)
				{ // fd is available for read
					std::cout << "in " << monitoredIt->fd << std::endl;
					MapServer::iterator serverIt = this->servers.find(monitoredIt->fd);
					if (serverIt != this->servers.end())
					{
						TcpServer *server = serverIt->second;
						this->connectServerToClient(server);
						--numReadyEvents;
						++monitoredIt;
						continue;
					}
					MapClient::iterator clientIt = this->clients.find(monitoredIt->fd);
					if (clientIt != this->clients.end())
					{
						http::Client *client = clientIt->second;
						this->handleClientRequest(client, monitoredIt);
						--numReadyEvents;
						++monitoredIt;
						continue;
					}
					MapFileReads::iterator fileReadsIt =
						this->fileReads.find(monitoredIt->fd);
					if (fileReadsIt != this->fileReads.end())
					{
						http::Client *client = fileReadsIt->second;
						this->handleFileReads(client, monitoredIt);
						--numReadyEvents;
						++monitoredIt;
						continue;
					}
				} else if (monitoredIt->revents & POLLOUT)
				{ // fd is available for write
					//std::cout << "out "  << monitoredIt->fd << std::endl;
					MapClient::iterator clientIt = this->clients.find(monitoredIt->fd);
					if (clientIt != this->clients.end())
					{
						http::Client *client = clientIt->second;
						this->handleClientWriteResponse(client, monitoredIt);
						--numReadyEvents;
						++monitoredIt;
						continue;
					}
				}
				++monitoredIt;
			}
		}
	}
}
