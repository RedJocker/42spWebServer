/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:43:15 by maurodri          #+#    #+#             */
/*   Updated: 2025/10/06 20:02:32 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include <iostream>

namespace http {

	Client::Client(): conn::TcpClient(0) {}

	Client::Client(int clientFd, Server *server): conn::TcpClient(clientFd), server(server) {}

	Client::Client(const Client &other): conn::TcpClient(other) {
		//TODO
	}

	Client &Client::operator=(const Client &other) {
		if (this == &other)
			return *this;
		//TODO

		return *this;
	}

	Client::~Client() {

	}

	Request &Client::readHttpRequest() {
		this->request.readHttpRequest(this->reader);
		return this->request;
	}

	Response &Client::getResponse() {
		return this->response;
	}

	Request &Client::getRequest() {
		return this->request;
	}

	Server *Client::getServer() const {
		return this->server;
	}
	
	void Client::setServer(Server *srv) {
		this->server = srv;
	}

	void Client::clear()
	{
		this->clearReadOperation();
		this->request.clear();
		this->response.clear();
	}
}
