// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   BufferedReader.cpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/21 21:13:05 by maurodri          #+#    #+#             //
//   Updated: 2025/08/23 00:39:02 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "BufferedReader.hpp"
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <iostream>

BufferedReader::BufferedReader()
{
	throw std::domain_error("should call constructor with fd");
}

BufferedReader::BufferedReader(int fd)
	: fd(fd), readBefore(0), buffered(std::vector<char>())
{

}

BufferedReader::BufferedReader(BufferedReader &toCopy)
{
	*this = toCopy;
}

BufferedReader BufferedReader::operator=(BufferedReader &other)
{
	if (this == &other)
		return *this;
	this->fd = other.fd;
	std::memcpy(this->readBuffer, other.readBuffer, BUFFER_SIZE);
	this->buffered = other.buffered;
	this->readBefore = other.readBefore;
	return *this;
}


BufferedReader::~BufferedReader()
{

}

// returns allocated char* and reset read state
char *BufferedReader::consumeBufferedContent()
{
	char *message = new char[this->buffered.size() + 1];
	::memcpy(message, &this->buffered[0], this->buffered.size());
	message[this->buffered.size()] = '\0';
	this->buffered.clear();
	this->readBefore = 0;
	return message;
}

std::pair<ReadState, char *> BufferedReader::read(size_t length)
{
	size_t toRead = length > this->readBefore ? length - this->readBefore : 0;
	if (toRead == 0)
	{
		return std::make_pair(
			ERROR,
			const_cast<char *>("called with no content to read"));
	}
	ssize_t currentRead = ::read(
		this->fd,
		this->readBuffer,
		std::min(toRead, static_cast<size_t>(BUFFER_SIZE)));
	if (currentRead < 0)
	{
		return std::make_pair(
			ERROR,
			const_cast<char *>("read returned negative"));
	}
	else if (currentRead == 0)
	{
		char *message = this->consumeBufferedContent();
		return std::make_pair(
			NO_CONTENT,
			message
		);
	}
	if (static_cast<size_t>(currentRead) > toRead)
		throw std::domain_error("unexpected read bigger than toRead");
	if (static_cast<size_t>(currentRead) == toRead)
	{
		this->buffered.insert(
			this->buffered.end(),
			readBuffer,
			readBuffer + toRead);
		char *message = this->consumeBufferedContent();
		return std::make_pair(DONE, message);
	}
	else // if (currentRead < toRead && currentRead > 0)
	{
		this->buffered.insert(
			this->buffered.end(),
			readBuffer,
			readBuffer + currentRead);
		this->readBefore += currentRead;
		return std::make_pair(READING, reinterpret_cast<char *>(0));
	}
}

std::pair<ReadState, char *> BufferedReader::readlineCrlf()
{
	ssize_t currentRead = ::read(
		this->fd, this->readBuffer, BUFFER_SIZE);

	if (currentRead < 0)
	{
		return std::make_pair(
			ERROR,
			const_cast<char *>("read returned negative"));
	}
	else if (currentRead == 0)
	{
		char *message = this->consumeBufferedContent();
		return std::make_pair(
			NO_CONTENT,
			message
		);
	}

	int index_crlf = -1;
	for (int i = 0; i < currentRead - 1; ++i)
	{
		if (this->readBuffer[i] == '\r' && this->readBuffer[i + 1] == '\n')
		{
			index_crlf = i;
			break;
		}
	}
	if (this->readBuffer[0] == '\n' && *(this->buffered.end() - 1) == '\r')
	{ // \r was last buffered and \n first on currentRead
		this->buffered.pop_back();
		char *messageLineCrlf = this->consumeBufferedContent();
		this->buffered.insert(
			this->buffered.end(),
			readBuffer + 1,
			readBuffer + currentRead);
		return std::make_pair(
			DONE,
			messageLineCrlf);
	}
	else if (index_crlf < 0)
	{
		this->buffered.insert(
			this->buffered.end(),
			readBuffer,
			readBuffer + currentRead);
		return std::make_pair(READING, reinterpret_cast<char *>(0));
	}
	this->buffered.insert(
			this->buffered.end(),
			readBuffer,
			readBuffer + index_crlf);
	char *messageLineCrlf = this->consumeBufferedContent();
	this->buffered.insert(
		this->buffered.end(),
		readBuffer + index_crlf + 2,
		readBuffer + currentRead);

	return std::make_pair(
		DONE,
		messageLineCrlf);
}
