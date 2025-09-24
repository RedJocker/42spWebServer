/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 17:06:06 by maurodri          #+#    #+#             */
//   Updated: 2025/09/23 19:22:32 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"
#include "BufferedWriter.hpp"
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


	bool EventLoop::subscribeHttpServer(http::Server *server)
	{
		struct pollfd event;
		event.events = POLLIN; // subscribe for reads only
		event.fd = server->getServerFd();

		this->events.push_back(event);

		servers.insert(std::make_pair(server->getServerFd(), server));

		return true;
	}

	bool EventLoop::subscribeHttpClient(int fd, http::Server *server)
	{

		struct pollfd event;

		event.events = POLLIN|POLLOUT; // subscribe for reads and writes
		event.fd = fd;

		http::Client *httpClient = new http::Client(fd, server);
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

	void EventLoop::connectServerToClient(http::Server *server)
	{
		std::pair<int, std::string> connResult = server->connectToClient();
		int clientFd = connResult.first;
		if (clientFd < 0)
		{
			std::string errorMessage = connResult.second;
			std::cout << errorMessage << std::endl;
			return;
		}
		bool isSubscribed = this->subscribeHttpClient(clientFd, server);
		if (!isSubscribed)
		{
			std::string errorMessage = "failed to subscribe client";
			std::cout << errorMessage << std::endl;
			close(clientFd);
		} else {
			std::cout << "connected client " << clientFd << " on server " << server->getPort() << std::endl;
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
			struct pollfd event;
			event.events = POLLOUT; // subscribe for writes
			event.fd = fileFd;
			events.push_back(event);
			client->setOperationFd(fileFd, content);
			this->fileWrites.insert(std::make_pair(fileFd, client));
		}
	}

	void EventLoop::handleFileWrite(
		http::Client &client,  ListEvents::iterator &eventIt)
	{
		std::cout << "clientFd = " << client.getFd() << std::endl;
		std::cout << "handleFileWrite: " << eventIt->fd << std::endl;

		if (client.getWriterState() != BufferedWriter::WRITING)
		{
			throw std::domain_error("called handleFileWrite without"
									"content to write");
		}

		std::pair<WriteState, char*> writeResult =
			client.flushOperation();

		if (writeResult.first == BufferedWriter::WRITING)
		{
			return ;
		}
		if (writeResult.first == BufferedWriter::DONE)
		{
			std::cout << "file writing done" << eventIt->fd << std::endl;
			client.clearWriteOperation();
			client.getResponse()
				.setCreated();
			client.setMessageToSend(client.getResponse().toString());
		} else
		{
			std::cout << "file writing error "
					  << writeResult.second
					  << " "
					  << eventIt->fd << std::endl;
			client.getResponse()
				.setInternalServerError();
			client.setMessageToSend(client.getResponse().toString());
		}

		this->unsubscribeFd(eventIt->fd);
		this->fileWrites.erase(this->fileWrites.find(eventIt->fd));
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
		client->clearReadOperation();
	}

	void EventLoop::handleClientRequest(
		http::Client *client, ListEvents::iterator &eventIt)
	{
		http::Request maybeCompleteRequest = client->readHttpRequest();

		switch (maybeCompleteRequest.state()) {
			case http::Request::READING_REQUEST_LINE:
			case http::Request::READING_HEADERS:
			case http::Request::READING_BODY:
				// has not finished reading, will finish a next epoll_wait
				return;

			case http::Request::READ_COMPLETE: {
				// has finished reading has message and client still alive
				http::Request &request = maybeCompleteRequest;
				std::cout << "done reading: "
					  << request.getMethod() << " "
					  << request.getPath() << " "
					  << request.getProtocol() << " "
					  << std::endl;

				dispatcher.dispatch(*client, *this);

				if (request.getHeader("Connection") == "close") {
					std::cout << "Request requested Connection: close, scheduling shutdown" << std::endl;
					unsubscribeHttpClient(eventIt);
				}

				break;
			}

			case http::Request::READ_BAD_REQUEST: {
				// has finished reading has message and client still alive
				std::cout << "bad request reading: "
					  << std::endl;
				std::string messageToSend =
					client->getResponse()
					.setBadRequest()
					.toString();
				client->setMessageToSend(messageToSend);

				// Close connection after bad request
				unsubscribeHttpClient(eventIt);
				break;
			}

			case http::Request::READ_EOF: {
				// has finished reading has message but client has closed
				std::cout << "eof reading: " << std::endl;
				unsubscribeHttpClient(eventIt);
				break;
			}

			case http::Request::READ_ERROR: {
				// reading failed for some reason
				//unsubscribe
				std::cout << "error reading: "
						<< std::endl;
				unsubscribeHttpClient(eventIt);
				throw std::domain_error("TODO read ERROR");
			}

			default:
				throw std::domain_error("unknown request state");
		}
	}

	void EventLoop::handleClientWriteResponse(
		http::Client *client, ListEvents::iterator &eventIt)
	{
		if(client->getWriterState() != BufferedWriter::WRITING)
			throw std::domain_error("called handleClientWriteResponse without content to write");

		std::pair<WriteState, char*> flushResult = client->flushMessage();

		// Check if client requested connection close
		bool requestClose = client->getRequest().getHeader("Connection") == "close";

		if (flushResult.first == BufferedWriter::DONE) {
			// Add Connection: close header if the client requested it
			if (requestClose)
				client->getResponse().addHeader("Connection", "close");

			// Clear client request/response for next request
			client->clear();
		} else if (flushResult.first == BufferedWriter::ERROR) {
			// Handle write errors
			throw std::domain_error("write ERROR");
			client->clear();
		}

		// Unsubscribe the client if connection must close
		if (requestClose)
			unsubscribeHttpClient(eventIt);
	}

	void EventLoop::handleFdEvent(ListEvents::iterator &monitoredIt)
	{
		//std::cout << "monitoredFd  " << monitoredIt->fd << std::endl;
		if (monitoredIt->revents & (POLLHUP | POLLERR))
		{ // close
			std::cout << "close: " << monitoredIt->fd << std::endl;
			MapClient::iterator clientIt = this->clients.find(monitoredIt->fd);
			if (clientIt != this->clients.end())
			{
				this->unsubscribeHttpClient(monitoredIt);
				return;
			}
		}
		if (monitoredIt->revents & POLLOUT)
		{ // fd is available for write
			//std::cout << "out "  << monitoredIt->fd << std::endl;
			MapFileWrites::iterator fileWritesIt =
				this->fileWrites.find(monitoredIt->fd);
			if (fileWritesIt != this->fileWrites.end())
			{
				http::Client *client = fileWritesIt->second;
				if (client) {
					this->handleFileWrite(*client, monitoredIt);
				}
				return;
			}
			MapClient::iterator clientIt = this->clients.find(monitoredIt->fd);
			if (clientIt != this->clients.end())
			{
				http::Client *client = clientIt->second;
				if (client->getOperationFd() < 0
					&& client->getWriterState() == BufferedWriter::WRITING)
				{
					this->handleClientWriteResponse(client, monitoredIt);
					return;
				}
			}
		}
		if (monitoredIt->revents & POLLIN)
		{ // fd is available for read
			std::cout << "in: " << monitoredIt->fd << std::endl;
			MapServer::iterator serverIt = this->servers.find(monitoredIt->fd);
			if (serverIt != this->servers.end())
			{
				http::Server *server = serverIt->second;
				this->connectServerToClient(server);
				return;
			}
			MapClient::iterator clientIt = this->clients.find(monitoredIt->fd);
			if (clientIt != this->clients.end())
			{
				http::Client *client = clientIt->second;
				this->handleClientRequest(client, monitoredIt);
				return;
			}
			MapFileReads::iterator fileReadsIt =
				this->fileReads.find(monitoredIt->fd);
			if (fileReadsIt != this->fileReads.end())
			{
				http::Client *client = fileReadsIt->second;
				this->handleFileReads(client, monitoredIt);
				return;
			}
		}
	}

	void EventLoop::shutdown(void)
	{
		for (ListEvents::iterator monitoredIt = events.begin();
			 monitoredIt < events.end();
			 monitoredIt++)
		{
			close(monitoredIt->fd);
		}
		events.clear();
		servers.clear();
		for (MapClient::iterator clientIt = clients.begin();
			 clientIt != clients.end();
			 ++clientIt)
		{
			http::Client *client = clientIt->second;
			if (client)
			{
				delete client;
			}
		}
		clients.clear();
		fileReads.clear();
		fileWrites.clear();
		removeFds.clear();
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
				if (monitoredIt->revents & (POLLHUP | POLLERR | POLLOUT | POLLIN))
				{// some event of this fd
					handleFdEvent(monitoredIt);
					--numReadyEvents;
				}
				++monitoredIt;
			}
		}
	}
}
