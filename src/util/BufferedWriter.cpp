// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   BufferedWriter.cpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/25 22:58:25 by maurodri          #+#    #+#             //
//   Updated: 2025/08/26 21:31:07 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "BufferedWriter.hpp"
#include <unistd.h>
#include <stdexcept>

BufferedWriter::BufferedWriter()
{

}

BufferedWriter::BufferedWriter(int fd) : fd(fd), state(DONE), toWrite("")
{
}

BufferedWriter::BufferedWriter(const BufferedWriter &other)
	: fd(other.fd), state(other.state), toWrite(other.toWrite)
{

}

BufferedWriter &BufferedWriter::operator=(const BufferedWriter &other)
{
	if(this == &other)
		return *this;
	this->fd = other.fd;
	this->state = other.state;
	this->toWrite = other.toWrite;
	return *this;
}

BufferedWriter::~BufferedWriter()
{

}

void BufferedWriter::setMessage(std::string message)
{
	if (this->state != BufferedWriter::DONE)
		throw std::domain_error("setMessage can only be called when state is DONE");
	this->message = message;
	this->toWrite = message;
	this->state = BufferedWriter::WRITING;
}

std::string BufferedWriter::getMessage() const
{
	return this->message;
}

WriteState BufferedWriter::getState() const
{
	return this->state;
}

std::pair<WriteState, char *>  BufferedWriter::flushMessage()
{
	if (this->state != BufferedWriter::WRITING)
		throw std::domain_error(
			"flushMessage can only be called when state is WRITING");
	ssize_t sizeToWrite = static_cast<ssize_t>(this->toWrite.size());
	const char *message = this->toWrite.c_str();
	ssize_t sizeWritten = write(this->fd, message, sizeToWrite);
	if (sizeWritten < 0)
	{
		this->state = BufferedWriter::ERROR;
		return std::make_pair(
			this->state, const_cast<char *>("write returned negative number"));
	}
	else if (sizeWritten < sizeToWrite)
	{
		this->toWrite = this->toWrite.substr(sizeWritten);
		return std::make_pair(this->state, reinterpret_cast<char *>(0));
	}
	else
	{
		this->state = BufferedWriter::DONE;
		return std::make_pair(this->state, reinterpret_cast<char *>(0));
	}
}
