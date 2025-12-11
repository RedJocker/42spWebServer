/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:43:15 by maurodri          #+#    #+#             */
//   Updated: 2025/12/11 08:02:49 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "pathUtils.hpp"
#include <iostream>
#include "Route.hpp"
#include "VirtualServer.hpp"

namespace http {

	Client::Client()
		: conn::TcpClient(0),
		  server(0),
		  vserver(0),
		  route(0),
		  request(),
		  response(),
		  pendingFileWrites(0) {}

	Client::Client(int clientFd, Server *server)
		: conn::TcpClient(clientFd),
		  server(server),
		  vserver(0),
		  route(0),
		  request(),
		  response(),
		  pendingFileWrites(0){}

	Client::Client(const Client &other): conn::TcpClient(other),
		server(other.server),
		vserver(other.vserver),
		route(other.route),
		request(other.request),
		response(other.response),
		pendingFileWrites(other.pendingFileWrites)
	{
	}

	Client &Client::operator=(const Client &other) {
		if (this != &other) {
			conn::TcpClient::operator=(other);
			server = other.server;
			vserver = other.vserver;
			route = other.route;
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

	void Client::setVirtualServer(VirtualServer *vserver)
	{
		this->vserver = vserver;
	}

	void Client::setRoute(Route *routeToServeClientRequest) {
		this->route = routeToServeClientRequest;
	}

	void Client::clear()
	{
		this->vserver = 0;
		this->route = 0;
		this->request.clear();
		this->response.clear();
		this->pendingFileWrites = 0;
	}

	void Client::writeResponse(void)
	{
		MapErrorPages const *errorPages = 0;
		std::string message;
		if (this->route) {
			message = "using route "
				+ this->route->getPathSpecification()
				+ " error pages";
			errorPages = &(this->route->getErrorPages());
		}
		else if (this->vserver) {
			message = "using virtual server "
				+ this->vserver->getHostname()
				+ " error pages";
			errorPages = &(this->vserver->getErrorPages());
		}
		else if (this->server) {
			message = "using server "
				+ this->server->getAddressPort()
				+ " error pages";
			errorPages = &(this->server->getErrorPages());
		}

		Response &response = this->response;

		if (errorPages && response.isBodyEmpty())
		{
			unsigned short int status =
				static_cast<unsigned short int>(response.getStatusCode());
			MapErrorPages::const_iterator it = errorPages->find(status);
			if (it != errorPages->end())
			{
				std::cout << message << " for status " << status << std::endl;
				response.setBody(it->second);
			}
		}
		if (this->getRequest().getHeader("Connection") == "close")
			this->getResponse().addHeader("Connection", "close");
		if (response.isBodyEmpty() && this->getResponse().getHeader("Content-Lenght") == "")
		{
			this->getResponse().addHeader("Content-Length", "0");
		}
		this->setMessageToSend(response.toString());
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

	time_t Client::getCgiTimeout(void)
	{
		if (this->route)
			return this->route->getCgiTimeout();
		else // if route is null disconnect
			return 0;
	}
}
