/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:43:15 by maurodri          #+#    #+#             */
/*   Updated: 2025/09/02 16:28:24 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <sstream>

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
		Response r = response;

		if (this->request.state() == http::Request::READ_BAD_REQUEST) {
			r.setStatusCode(400);
			r.setStatusInfo("Bad Request");
		} else if (this->request.state() == http::Request::READ_ERROR) {
			r.setStatusCode(500);
			r.setStatusInfo("Internal Server Error");
		} else if (this->request.getMethod() == "TRACE") {
			r.setStatusCode(200);
			r.setStatusInfo("OK");
			r.setBody(this->request.getBody());
		} else {
			r.setStatusCode(404);
			r.setStatusInfo("Not Found");
		}

		std::ostringstream length;
		length << r.getBody().size();
		r.addHeader("Content-Length", length.str());

		return r.toString();
	}

	void Client::clear()
	{
		this->request.clear();
		this->response.clear();
	}
}
