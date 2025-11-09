/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:43:15 by maurodri          #+#    #+#             */
//   Updated: 2025/11/08 01:10:38 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include <iostream>

namespace http {

	Client::Client(): conn::TcpClient(0), pendingFileWrites(0) {}

	Client::Client(int clientFd, Server *server)
		: conn::TcpClient(clientFd), server(server), pendingFileWrites(0){}

	Client::Client(const Client &other): conn::TcpClient(other),
		server(other.server),
		request(other.request),
		response(other.response),
		pendingFileWrites(other.pendingFileWrites)
	{
	}

	Client &Client::operator=(const Client &other) {
		if (this != &other) {
			conn::TcpClient::operator=(other);
			server = other.server;
			request = other.request;
			response = other.response;
			pendingFileWrites = other.pendingFileWrites;
		}
		return *this;
	}

	Client::~Client() {

	}

	Request &Client::readHttpRequest() {
		this->request.readHttpRequest(this->reader);
		if (this->request.state() == Request::READ_COMPLETE)
			this->request.getPath().initRequestPath(this->request.getPathRaw());
		return this->request;
	}

	Response &Client::getResponse() {
		return this->response;
	}

	Request &Client::getRequest() {
		return this->request;
	}

	Server *Client::getServer(void) const {
		return this->server;
	}

	Route *Client::getRoute(void) const {
		return this->route;
	}

	void Client::setRoute(Route *routeToServeClientRequest) {
		this->route = routeToServeClientRequest;
	}

	void Client::clear()
	{
		this->request.clear();
		this->response.clear();
		this->pendingFileWrites = 0;
	}

	void Client::pendingFileWritesIncrement(void)
	{
		++this->pendingFileWrites;
	}
	void Client::pendingFileWritesDecrement(void)
	{
		if (this->pendingFileWrites > 0)
			--this->pendingFileWrites;
	}
	size_t Client::getPendingFileWrites(void) const
	{
		return this->pendingFileWrites;
	}
}
