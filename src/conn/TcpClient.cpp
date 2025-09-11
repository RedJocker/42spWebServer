// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   TcpClient.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/25 21:44:37 by maurodri          #+#    #+#             //
//   Updated: 2025/09/11 01:56:46 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "TcpClient.hpp"
#include <unistd.h>
#include <stdexcept>
#include <iostream>

namespace conn
{

	TcpClient::TcpClient(): clientFd(0), operationFd(-1),  reader(0), writer(0)
	{
		throw std::domain_error(
			"default constructor is not allowed, use constructor with clientFd parameter");
	}

	TcpClient::TcpClient(int clientFd)
		: clientFd(clientFd), operationFd(-1), reader(clientFd), writer(clientFd)
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

	std::pair<ReadState, char *> TcpClient::readAllOperationFd()
	{
		this->reader.setFd(this->operationFd);
		return this->reader.readAll();
	}

	void TcpClient::setMessageToSend(std::string message)
	{
		std::cout << "setMessage: " << message <<std::endl;
		this->writer.setMessage(message);
	}

	WriteState TcpClient::getWriterState() const
	{
		return this->writer.getState();
	}

	std::string TcpClient::getMessageToSend() const
	{
		return this->writer.getMessage();
	}

	std::pair<WriteState, char*> TcpClient::flushMessage()
	{
		return this->writer.flushMessage();
	}

	bool TcpClient::hasBufferedContent() const
	{
		return this->reader.hasBufferedContent();
	}

	int TcpClient::getFd() const
	{
		return this->clientFd;
	}

	int TcpClient::getOperationFd() const
	{
		return this->operationFd;
	}
	void TcpClient::setOperationFd(int operationFd)
	{
		this->operationFd = operationFd;
	}

	void TcpClient::clearReadOperation()
	{
		this->reader.setFd(clientFd);
	}
}
