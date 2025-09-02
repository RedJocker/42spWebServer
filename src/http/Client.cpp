/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:43:15 by maurodri          #+#    #+#             */
/*   Updated: 2025/09/02 14:32:38 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

namespace http {

	Client::Client(): conn::TcpClient(0)
	{
	}

	Client::Client(int clientFd): conn::TcpClient(clientFd)
	{
	}

	Client::Client(const Client &other): conn::TcpClient(other)
	{
		//TODO
	}

	Client &Client::operator=(const Client &other)
	{
		if (this == &other)
			return *this;
		//TODO

		return *this;
	}

	Client &Client::operator=(const conn::TcpClient &other) {
			if (this == &other) {
				return *this;
			}
			conn::TcpClient::operator=(other);
			return *this;
	}

	Client::~Client()
	{
		//TODO
	}

	Request Client::readHttpRequest()
	{
		this->request.readHttpRequest(*this);
		return this->request;
	}

	std::string Client::responseAsString() const
	{
		http::Response response;
		if (this->request.state() == http::Request::READ_BAD_REQUEST) {
			response.setStatusCode(400);
			response.setStatusInfo("Bad Request");
		} else if (this->request.state() == http::Request::READ_ERROR) {
			response.setStatusCode(500);
			response.setStatusInfo("Internal Server Error");
		} else {
			response.setStatusCode(404);
			response.setStatusInfo("Not found");
		}

		response.addHeader("Content-Length", "0");

		return response.toString();
	}

	void Client::clear()
	{
		this->request.clear();
		this->response.clear();
	}
}
