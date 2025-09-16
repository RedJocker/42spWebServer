/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:43:15 by maurodri          #+#    #+#             */
//   Updated: 2025/09/16 18:11:59 by maurodri         ###   ########.fr       //
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

	Client::~Client()
	{
		//TODO
	}

	Request &Client::readHttpRequest()
	{
		this->request.readHttpRequest(this->reader);
		return this->request;
	}

	Response &Client::getResponse()
	{
		return this->response;
	}

	Request &Client::getRequest()
	{
		return this->request;
	}

	void Client::clear()
	{
		this->clearReadOperation();
		this->request.clear();
		this->response.clear();
	}
}
