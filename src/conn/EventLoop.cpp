/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 17:06:06 by maurodri          #+#    #+#             */
//   Updated: 2025/10/01 23:59:29 by maurodri         ###   ########.fr       //
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

#include "Headers.hpp"

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

		for (MapOperations::iterator it = operations.begin();
			 it != operations.end();
			 it++)
		{
			if (it->second->getFd() == clientFd)
			{
				close(it->first.fd);
				unsubscribeFd(it->first.fd);
				operations.erase(it);
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
			std::string errorMessage = "failed to subscribe client ";
			std::cout << errorMessage << std::endl;
			close(clientFd);
		} else {
			std::cout << "connected client " << clientFd
					  << " on server " << server->getPort() << std::endl;
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
			Operation op = {Operation::FILE_READ, fileFd};
			this->operations.insert(std::make_pair(op, client));
		}
	}

	void EventLoop::subscribeFileWrite(
		int fileFd, int clientFd, std::string content)
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
			Operation op = {Operation::FILE_WRITE, fileFd};
			this->operations.insert(std::make_pair(op, client));
		}
	}

	void EventLoop::subscribeCgi(int cgiFd, int clientFd)
	{

		http::Client *clientPtr = this->clients.at(clientFd);
		if (clientPtr)
		{
			http::Client &client = *clientPtr;
			std::cout << "subscribeCgi " << cgiFd << std::endl;
			struct pollfd event;
			event.events = POLLIN | POLLOUT; // subscribe for reads and writes
			event.fd = cgiFd;
			events.push_back(event);
			client.setOperationFd(cgiFd);
			Operation op = {Operation::CGI, cgiFd};
			this->operations.insert(std::make_pair(op, clientPtr));

			std::cout << "parent writing to cgi: " << std::endl;
			BufferedWriter writer(cgiFd);
			writer.setMessage("hello=there&abc=def");
			std::pair<WriteState, char *> flushResult(
				BufferedWriter::WRITING, 0);
			std::cout << writer.getState() << std::endl;
			while (flushResult.first == BufferedWriter::WRITING)
				flushResult = writer.flushMessage();
			//shutdown(cgiFd,SHUT_WR);

			// Read CGI response
			std::cout << "parent done writing to cgi:" << std::endl;
			BufferedReader reader(cgiFd);
			std::pair<ReadState, char *> readResult;
			while(readResult.first == BufferedReader::READING)
			{
				std::cout << "parent reading" << std::endl;
				readResult = reader.readAll();
			}
			std::cout << "parent done reading" << std::endl;
			if (readResult.first != BufferedReader::NO_CONTENT)
			{
				std::cout << "error exit" << std::endl;
				// TODO error on cgi reading
				client.getResponse().setInternalServerError();
				client.setMessageToSend(client.getResponse().toString());
				close(cgiFd);
				return ;
			}

			std::string cgiResponseString(readResult.second);
			delete[] readResult.second;
			close(cgiFd);
			client.clearReadOperation();
			client.clearWriteOperation();
			std::cout << "CGI Response: "<< cgiResponseString << std::endl;

			// Parse headers and body
			size_t separatorIndex = cgiResponseString.find("\r\n\r\n");
			if (separatorIndex == std::string::npos)
			{
				client.getResponse().setOk().setBody(cgiResponseString);
				client.setMessageToSend(client.getResponse().toString());
			}
			else
			{
				std::string cgiHeadersStr = cgiResponseString
					.substr(0, separatorIndex);
				http::Headers &cgiHeaders = client.getResponse().headers();

				size_t index = 0;
				while (1)
				{
					size_t index_next = cgiHeadersStr.find("\r\n", index);
					std::string headerLine = (index_next == std::string::npos)
						? cgiHeadersStr.substr(index)
						: cgiHeadersStr.substr(index, index_next - index);

					if (!cgiHeaders.parseLine(headerLine))
					{
						client.getResponse().setInternalServerError();
						client.setMessageToSend(client.getResponse().toString());
					}
					if (index_next == std::string::npos)
						break;
					index = index_next + 2;
				}
				client.getResponse()
					.setOk()
					.setBody(cgiResponseString.substr(separatorIndex + 4));
				client.setMessageToSend(client.getResponse().toString());
			}
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
			http::Server *server = client.getServer();
			if (server)
				server->onFileWritten(client);
		} else
		{
			std::cout << "file writing error "
					  << writeResult.second
					  << " "
					  << eventIt->fd << std::endl;
			http::Server *server = client.getServer();
			if (server)
				server->onServerError(client);
		}

		this->unsubscribeFd(eventIt->fd);
		Operation op = {Operation::FILE_WRITE, eventIt->fd};
		this->operations.erase(this->operations.find(op));
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
			delete[] readResult.second;
			http::Server *server = client->getServer();
			if (server)
				server->onFileRead(*client, responseStr);

		} else
		{
			std::cout << "failed handleFileReads "
					  << readResult.second << std::endl;
		}

		this->unsubscribeFd(eventIt->fd);
		Operation op = {Operation::FILE_READ, eventIt->fd};
		this->operations.erase(this->operations.find(op));
		client->clearReadOperation();
	}

	void EventLoop::handleClientRequest(
		http::Client *client, ListEvents::iterator &eventIt)
	{
		http::Request &req = client->readHttpRequest();

		switch (req.state()) {
			case http::Request::READING_REQUEST_LINE:
			case http::Request::READING_HEADERS:
			case http::Request::READING_BODY:
				// has not finished reading, will finish a next epoll_wait
				return;

			case http::Request::READ_COMPLETE: {
				// has finished reading has message and client still alive
				std::cout << "done reading: "
					  << req.getMethod() << " "
					  << req.getPath() << " "
					  << req.getProtocol() << std::endl;

				dispatcher.dispatch(*client, *this);

				if (req.getHeader("Connection") == "close")
				{
					std::cout
						<< "Request requested Connection: close"
						<< std::endl;
					client->getResponse().addHeader("Connection", "close");
				}

				return;
			}

			case http::Request::READ_BAD_REQUEST: {
				// has finished reading has message and client still alive
				std::cout << "bad request reading" << std::endl;

				client->getResponse().setBadRequest();
				client->getResponse().addHeader("Connection", "close");
				client->setMessageToSend(client->getResponse().toString());

				return;
			}

			case http::Request::READ_EOF: {
				std::cout
					<< "eof reading: client closed connection" << std::endl;
				unsubscribeHttpClient(eventIt);
				return;
			}

			case http::Request::READ_ERROR: {
				std::cout << "error reading request" << std::endl;
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
		std::cout << "monitoredFd  " << monitoredIt->fd << std::endl;
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
			std::cout << "out "  << monitoredIt->fd << std::endl;
			Operation op = {Operation::ANY, monitoredIt->fd};
			MapOperations::iterator operationIt = this->operations.find(op);
			if (operationIt != this->operations.end()
				&& operationIt->first.type == Operation::FILE_WRITE)
			{
				http::Client *client = operationIt->second;
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
			Operation op = {Operation::ANY, monitoredIt->fd};
			MapOperations::iterator operationIt =
				this->operations.find(op);
			if (operationIt != this->operations.end()
				&& operationIt->first.type == Operation::FILE_READ)
			{
				http::Client *client = operationIt->second;
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
		operations.clear();
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
