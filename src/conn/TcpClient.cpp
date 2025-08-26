// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   TcpClient.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/25 21:44:37 by maurodri          #+#    #+#             //
//   Updated: 2025/08/26 01:26:25 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "TcpClient.hpp"
#include <unistd.h>

namespace conn
{

	TcpClient::TcpClient(): clientFd(0), reader(0), writer(0)
	{
		throw std::domain_error("should call constructor with reader");
	}

	TcpClient::TcpClient(int clientFd)
		: clientFd(clientFd), reader(clientFd), writer(clientFd)
	{

	}

	TcpClient::TcpClient(const TcpClient &other)
		: clientFd(other.clientFd), reader(other.reader), writer(other.writer)
	{

	}

	TcpClient &TcpClient::operator=(const TcpClient &other)
	{
		if(this == &other)
			return *this;

		this->clientFd = other.clientFd;
		this->reader = other.reader;
		this->writer = other.writer;
		return *this;
	}

	TcpClient::~TcpClient()
	{
		close(this->clientFd);
	}

	std::pair<ReadState, char *> TcpClient::read(size_t length)
	{
		return this->reader.read(length);
	}

	std::pair<ReadState, char *> TcpClient::readlineCrlf()
	{
		return this->reader.readlineCrlf();
	}

	void TcpClient::setMessageToSend(std::string message)
	{
		this->writer.setMessage(message);
	}
	WriteState TcpClient::getWriterState() const
	{
		return this->writer.getState();
	}

	std::pair<WriteState, char*> TcpClient::flushMessage()
	{
		return this->writer.flushMessage();
	}
}
