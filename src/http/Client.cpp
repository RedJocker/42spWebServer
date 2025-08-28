// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Client.cpp                                         :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/27 17:43:15 by maurodri          #+#    #+#             //
//   Updated: 2025/08/27 21:34:52 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

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

	Request Client::readHttpRequest()
	{
		this->request.readFromTcpClient(*this);
		return this->request;
	}

	std::string Client::responseAsString() const
	{
		if (this->request.state() == http::Request::READ_BAD_REQUEST)
			return "HTTP/1.1 400 Bad Request\r\n\r\n";
		else if (this->request.state() == http::Request::READ_ERROR)
			return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
		return "HTTP/1.1 404 Not Found\r\n\r\n";
    }

    void Client::clear()
	{
		this->request.clear();
		// this->response.clear();
    }
}
