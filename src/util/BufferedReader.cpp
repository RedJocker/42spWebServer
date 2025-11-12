// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   BufferedReader.cpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/21 21:13:05 by maurodri          #+#    #+#             //
/*   Updated: 2025/11/11 18:48:59 by maurodri         ###   ########.fr       */
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

BufferedReader::BufferedReader(const BufferedReader &toCopy)
{
	*this = toCopy;
}

BufferedReader &BufferedReader::operator=(const BufferedReader &other)
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

bool BufferedReader::hasBufferedContent() const
{
	return this->buffered.size() > 0;
}

bool BufferedReader::bufferedContentCanMatch(
	const std::string &str) const
{
	size_t limit = std::min(str.size(), this->buffered.size());
	for (size_t i = 0; i < limit; ++i)
	{
		if (this->buffered.at(i) != str.at(i))
			return false;
	}
	return true;
}

ssize_t BufferedReader::crlfIndex(size_t start) const
{
	ssize_t len = this->buffered.size();
	ssize_t index_crlf = -1;
	for (ssize_t i = start; i < len - 1; ++i)
	{
		if (this->buffered.at(i) == '\r' && this->buffered.at(i + 1) == '\n')
		{
			index_crlf = i;
			break;
		}
	}
	return index_crlf;
}

// returns allocated char*
char *BufferedReader::consumeBufferedContent(size_t len, size_t eraseAfter)
{
	len = std::min(len, this->buffered.size());
	char *message = new char[len + 1];
	::memcpy(message, &this->buffered[0], len);
	message[len] = '\0';
	size_t toErase = std::min(len + eraseAfter, this->buffered.size());
	this->buffered.erase(buffered.begin(), buffered.begin() + toErase);
	this->readBefore = this->buffered.size();
	return message;
}

std::pair<ReadState, char *> BufferedReader::read(size_t length)
{
	size_t toRead = length > this->readBefore
		? length - this->readBefore
		: 0;

	if (toRead == 0)
	{
		char *message = this->consumeBufferedContent(length, 0);
		return std::make_pair(DONE, message);
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
		char *message = this->consumeBufferedContent(buffered.size(), 0);
		return std::make_pair(
			NO_CONTENT,
			message
		);
	}
	else if (static_cast<size_t>(currentRead) > toRead)
		throw std::domain_error("unexpected read bigger than toRead");

	this->buffered.insert(
			this->buffered.end(),
			readBuffer,
			readBuffer + currentRead);

	if (static_cast<size_t>(currentRead) == toRead)
	{
		char *message = this->consumeBufferedContent(length, 0);
		return std::make_pair(DONE, message);
	}
	else // if (currentRead < toRead && currentRead > 0)
	{
		this->readBefore += currentRead;
		return std::make_pair(READING, reinterpret_cast<char *>(0));
	}
}

std::pair<ReadState, char *> BufferedReader::readlineCrlf(void)
{
	ssize_t currentRead = 0;
	// read only if does not have buffered content
	ssize_t index_crlf_buffered = this->crlfIndex(0);
	size_t lenBeforeRead = this->buffered.size();
	if (index_crlf_buffered >=0)
	{
		char *messageLineCrlf =
			this->consumeBufferedContent(index_crlf_buffered, 2);

		return std::make_pair(
			DONE,
			messageLineCrlf);
	}
	else
	{
		currentRead = ::read(this->fd, this->readBuffer, BUFFER_SIZE);

		if (currentRead < 0)
		{ // read fail
			return std::make_pair(
				ERROR,
				const_cast<char *>("read returned negative"));
		}
		else if (currentRead == 0)
		{ // read eof
			char *message = this->consumeBufferedContent(buffered.size(), 0);
			return std::make_pair(
				NO_CONTENT,
				message);
		}
		// append current read to buffered
		this->buffered.insert(
			this->buffered.end(),
			readBuffer,
			readBuffer + currentRead);
	}
	ssize_t index_crlf = this->crlfIndex(
		lenBeforeRead < 1 ? 0 : lenBeforeRead - 1);
	if (index_crlf < 0)
		return std::make_pair(READING, reinterpret_cast<char *>(0));

	char *messageLineCrlf = this->consumeBufferedContent(index_crlf, 2);
	return std::make_pair(
		DONE,
		messageLineCrlf);
}

void BufferedReader::clear()
{
	this->buffered.clear();
	this->readBefore = 0;
}


std::pair<ReadState, char *> BufferedReader::readAll(void)
{

	ssize_t currentRead = ::read(
		this->fd,
		this->readBuffer,
		BUFFER_SIZE);

	if (currentRead < 0)
	{
		return std::make_pair(
			ERROR,
			const_cast<char *>("read returned negative"));
	}
	else if (currentRead == 0)
	{
		char *message = this->consumeBufferedContent(buffered.size(), 0);
		return std::make_pair(
			NO_CONTENT,
			message
		);
	}

	this->buffered.insert(
			this->buffered.end(),
			readBuffer,
			readBuffer + currentRead);

	return std::make_pair(READING, reinterpret_cast<char *>(0));
}
