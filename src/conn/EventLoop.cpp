/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 17:06:06 by maurodri          #+#    #+#             */
//   Updated: 2025/11/24 18:14:48 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"
#include "BufferedWriter.hpp"
#include "Monitor.hpp"
#include "devUtil.hpp"
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <signal.h>
#include <ctime>
#include <limits>

namespace conn
{

	bool EventLoop::shouldExit = false;

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

		this->subscribeFds.push_back(event);

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
			std::pair<MapClient::iterator, bool> insertResult =
				clients.insert(std::make_pair(event.fd, httpClient));
			this->subscribeFds.push_back(event);
			return insertResult.second;
		}
		return false;
	}

	void EventLoop::unsubscribeFd(int fd)
	{
		this->removeFds.insert(fd);
		// defer removing to approriate moment to avoid iterator invalidation
		// on EventLoop.loop
	}

	void EventLoop::unsubscribeOperation(int operationFd)
	{ // if changing this check also EventLoop.markExpiredOperations
		std::cout << "unsubscribeOperation " <<	 operationFd << std::endl;
		this->unsubscribeFd(operationFd);
		Operation op = Operation::matcher(operationFd);
		MapOperations::iterator opIt = this->operations.find(op);
		if (opIt != this->operations.end())
		{
			const Operation &opRef = opIt->first;
			if (opRef.writer)
				delete opRef.writer;
			if (opRef.reader)
				delete opRef.reader;
		}
		this->operations.erase(op);
	}

	void EventLoop::unsubscribeHttpClient(
		ListEvents::iterator &eventIt)
	{
		std::cout << "unsubscribeHttpClient " <<  eventIt->fd << std::endl;
		int clientFd = eventIt->fd;

		for (MapOperations::iterator it = operations.begin();
			 it != operations.end();)
		{
			if (it->second->getFd() == clientFd)
			{
				unsubscribeFd(it->first.fd);
				operations.erase(it++);
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
					  << " on server " << server->getAddressPort() << std::endl;
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
			this->subscribeFds.push_back(event);

			std::pair<Operation, http::Client *> entry =
				std::make_pair(Operation::declare(Operation::FILE_READ,
												  fileFd,
												  client->getCgiTimeout()),
							   client);
			this->operations.insert(entry);
		}
	}

	void EventLoop::subscribeFileWrite(
		int fileFd, int clientFd, std::string content)
	{
		http::Client *client = this->clients.at(clientFd);
		if (client)
		{
			client->pendingFileWritesIncrement();
			std::cout << "subscribeFileWrite " << fileFd << std::endl;
			struct pollfd event;
			event.events = POLLOUT; // subscribe for writes
			event.fd = fileFd;
			this->subscribeFds.push_back(event);
			Operation op =
				Operation::declare(Operation::FILE_WRITE, fileFd, client->getCgiTimeout());
			op.writer->setMessage(content);
			this->operations.insert(std::make_pair(op, client));
		}
	}

	void EventLoop::subscribeCgi(int cgiFd, pid_t cgiPid, int clientFd)
	{
		http::Client *clientPtr = this->clients.at(clientFd);
		if (clientPtr)
		{
			http::Client &client = *clientPtr;
			std::cout << "subscribeCgi " << cgiFd << std::endl;
			struct pollfd event;
			event.events = POLLIN | POLLOUT; // subscribe for reads and writes
			event.fd = cgiFd;
			this->subscribeFds.push_back(event);
			Operation op =
				Operation::declare(Operation::CGI, cgiFd, client.getCgiTimeout());
			op.cgiPid = cgiPid;
			std::string body = client.getRequest().getBody();
			if (client.getRequest().getMethod() == "POST" && !body.empty())
			{ // only write on posts with body
				op.writer->setMessage(client.getRequest().getBody());
			}

			this->operations.insert(std::make_pair(op, clientPtr));
		}
	}

	void EventLoop::handleFileWrite(const Operation &op, http::Client &client)
	{
		std::cout << "clientFd = " << client.getFd() << std::endl;
		std::cout << "handleFileWrite: " << op.fd << std::endl;

		if (op.writer->getState() != BufferedWriter::WRITING)
		{
			throw std::domain_error("called handleFileWrite without"
									"content to write");
		}

		std::pair<WriteState, char*> writeResult =
			op.writer->flushMessage();

		if (writeResult.first == BufferedWriter::WRITING)
		{
			return ;
		}
		if (writeResult.first == BufferedWriter::DONE)
		{
			std::cout << "file writing done" << op.fd << std::endl;
			client.pendingFileWritesDecrement();
			if (client.getPendingFileWrites() == 0)
			{
				http::Route *route = client.getRoute();
				if (route)
					route->respond(client, op);
			}
		} else
		{
			std::cout << "file writing error "
					  << writeResult.second
					  << " "
					  << op.fd << std::endl;
			http::Route *route = client.getRoute();
			if (route)
				route->onServerError(client);
		}

		this->unsubscribeOperation(op.fd);
	}

	void EventLoop::handleFileReads(const Operation &op, http::Client &client)
	{
		std::cout << "clientFd = " << client.getFd() << std::endl;
		std::cout << "handleFileReads" << std::endl;

		std::pair<BufferedReader::ReadState, char*> readResult
			= op.reader->readAll();

		if(readResult.first == BufferedReader::READING)
			return ;
		std::cout << "handleFileReads after reading" << std::endl;
		if (readResult.first == BufferedReader::NO_CONTENT)
		{
			op.content = std::string(readResult.second);
			delete[] readResult.second;
			http::Route *route = client.getRoute();
			if (route)
				route->respond(client, op);

		} else
		{
			std::cout << "failed handleFileReads "
					  << readResult.second << std::endl;
		}

		this->unsubscribeOperation(op.fd);
	}

	void EventLoop::handleCgiWrite(const Operation &op, http::Client &client)
	{
		if (op.writer->getState() != BufferedWriter::WRITING)
			return ;
		// Write CGI request
		std::cout << "parent writing to cgi: " << std::endl;

		std::pair<WriteState, char *> flushResult = op.writer->flushMessage();
		if (flushResult.first == BufferedWriter::WRITING)
			return;

		if (flushResult.first == BufferedWriter::ERROR)
		{
			std::cerr << "error cgi writing: " << strerror(errno) << std::endl;
			TODO("handle error writing to cgi process");
			http::Route *route = client.getRoute();
			if (route)
				route->onServerError(client);
		}
		std::cout << "parent done writing to cgi:" << std::endl;
		return;
	}

	void EventLoop::handleCgiRead(const Operation &op, http::Client &client)
	{
		if (op.writer->getState() == BufferedWriter::WRITING)
			return ;
		int cgiFd = op.fd;
		// Read CGI response
		std::pair<ReadState, char *> readResult = op.reader->readAll();
		if(readResult.first == BufferedReader::READING)
		{
			std::cout << "parent reading" << std::endl;
			return ;
		}
		std::cout << "parent done reading" << std::endl;
		if (readResult.first != BufferedReader::NO_CONTENT)
		{
			std::cerr << "error cgi reading: " << strerror(errno) << std::endl;
			this->unsubscribeOperation(cgiFd);
			client.getResponse().setInternalServerError();
			client.writeResponse();
			return ;
		}

		op.content = std::string(readResult.second);
		delete[] readResult.second;
		std::cout << "CGI Response: " << op.content << std::endl;

		http::Route *route = client.getRoute();
		if (route)
			route->respond(client, op);

		this->unsubscribeOperation(cgiFd);
	}

	void EventLoop::handleClientRequest(
		http::Client *client, ListEvents::iterator &eventIt)
	{
		if (client->getWriterState() != BufferedWriter::DONE)
			return ; // client is already being served
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
					  // << req.getMethod() << " "
					  // << req.getPath() << " "
					  // << req.getProtocol()
						  << req.toString()
						  << std::endl;

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
				client->writeResponse();

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

		if (flushResult.first == BufferedWriter::WRITING)
			return ; // still has content to write

		// Check if client requested connection close
		bool requestClose = client->getRequest().getHeader("Connection") == "close";

		if (flushResult.first == BufferedWriter::DONE) {
			// Add Connection: close header if the client requested it
			if (requestClose)
				client->getResponse().addHeader("Connection", "close");

		} else if (flushResult.first == BufferedWriter::ERROR) {
			// Handle write errors
			throw std::domain_error("write ERROR");
			client->clear();
		}

		bool responseClose = client->getResponse().getHeader("Connection") == "close";
		// Unsubscribe the client if connection must close
		if (requestClose || responseClose)
		{
			unsubscribeHttpClient(eventIt);
			return;
		}

		client->clear();
		if (client->hasBufferedContent())
		{ // BufferedReader has read content of more than one request
			std::cout << "buffered_in: " << client->getFd() << std::endl;
			this->handleClientRequest(client, eventIt);
			return;
		}
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
			// std::cout << "out "  << monitoredIt->fd << std::endl;
			Operation op = Operation::matcher(monitoredIt->fd);
			MapOperations::iterator operationIt = this->operations.find(op);
			if (operationIt != this->operations.end()
				&& operationIt->first.type == Operation::FILE_WRITE)
			{
				http::Client *client = operationIt->second;
				if (client) {
					this->handleFileWrite(operationIt->first, *client);
				}
				return;
			}
			if (operationIt != this->operations.end()
				&& operationIt->first.type == Operation::CGI)
			{
				http::Client *client = operationIt->second;
				if (client) {
					this->handleCgiWrite(operationIt->first, *client);
				}
			}
			MapClient::iterator clientIt = this->clients.find(monitoredIt->fd);
			if (clientIt != this->clients.end())
			{
				http::Client *client = clientIt->second;
				if (client->getWriterState() == BufferedWriter::WRITING)
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
			Operation op =
				Operation::matcher(monitoredIt->fd);
			MapOperations::iterator operationIt =
				this->operations.find(op);
			if (operationIt != this->operations.end()
				&& operationIt->first.type == Operation::FILE_READ)
			{
				http::Client *client = operationIt->second;
				this->handleFileReads(operationIt->first, *client);
				return;
			}
			if (operationIt != this->operations.end()
				&& operationIt->first.type == Operation::CGI)
			{
				http::Client *client = operationIt->second;
				if (client)
					this->handleCgiRead(operationIt->first, *client);
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
		for (MapServer::iterator serverIt = servers.begin();
			 serverIt != servers.end();
			 ++serverIt)
		{
			http::Server *server = serverIt->second;
			if (server)
				server->shutdown();
			delete server;
		}
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
		subscribeFds.clear();
		EventLoop::shouldExit = true;
	}

	time_t EventLoop::markExpiredOperations(void)
	{

		time_t minTimeout = std::numeric_limits<time_t>::max();
		for (MapOperations::iterator opIt = this->operations.begin();
			 opIt != this->operations.end();)
		{
			time_t expirationTime = opIt->first.timeToExpire();
			//std::cout << "expirationTime" << expirationTime << std::endl;
			if (expirationTime < 0)
			{
				std::cout << "expiredOp fd:" << opIt->first.fd << std::endl;
				http::Client *client = opIt->second;
				if (client && opIt->first.type == Operation::CGI
					&& opIt->first.writer->getState() != BufferedWriter::WRITING)
				{
					client->getResponse()
						.setGatewayTimeout();
					client->writeResponse();
					pid_t cgiPid = opIt->first.cgiPid;
					kill(cgiPid, SIGKILL);
				}
				// avoid call to unsubscribeOperation to avoid iterator invalidation
				if (opIt->first.writer)
					delete opIt->first.writer;
				if (opIt->first.reader)
					delete opIt->first.reader;
				this->unsubscribeFd(opIt->first.fd);
				this->operations.erase(opIt++);
			}
			else
			{
				minTimeout = expirationTime < minTimeout ?
					expirationTime : minTimeout;
				++opIt;
			}
		}
		return minTimeout;
}

	bool EventLoop::loop()
	{
		while (!EventLoop::shouldExit)
		{
			time_t minTimeout = this->markExpiredOperations();
			//std::cout << "minTimeout: " << minTimeout << std::endl;

			if (!this->subscribeFds.empty())
			{
				// complete pending subscriptions
				this->events.insert(
					this->events.end(),
					this->subscribeFds.begin(),
					this->subscribeFds.end());
				this->subscribeFds.clear();
			}
			// waiting for ready event from epoll
			// -1 without timeout
			// TODO make timeout system for clients
			int timeoutTime = static_cast<int>(minTimeout) * 1000;
			int numReadyEvents =
				poll(this->events.data(), events.size(), timeoutTime);
			if (numReadyEvents < 0)
			{
				std::cout << "No events on pool due to: "
						  << strerror(errno) << std::endl;
				continue;
			}
			else if (numReadyEvents == 0)
			{
				std::cout << "Timeout on poll: " << std::endl;
				continue;
			}

			for (ListEvents::iterator monitoredIt = this->events.begin();
				 monitoredIt != this->events.end() && numReadyEvents > 0;
				 )
			{
				// remove unsubscribed from last iteration
				SetRemoveFd::iterator removeIt =
					this->removeFds.find(monitoredIt->fd);
				if (removeIt != this->removeFds.end())
				{
					std::cout << "removing:  " << monitoredIt->fd << std::endl;
					::close(monitoredIt->fd);
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
		std::cout << "Server shuting down" << std::endl;
		this->shutdown();
		return true;
	}
}
