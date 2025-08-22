// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   BufferedReader.cpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/21 21:13:05 by maurodri          #+#    #+#             //
//   Updated: 2025/08/22 01:44:12 by maurodri         ###   ########.fr       //
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

std::pair<ReadState, char *> BufferedReader::read(size_t length)
{
	int toRead = static_cast<int>(length) - this->readBefore;
	if (toRead <= 0)
	{
		return std::make_pair(
			ERROR,
			const_cast<char *>("called with no content to read"));
	}
	int currentRead = ::read(
		this->fd, this->readBuffer, std::min(toRead, BUFFER_SIZE));
	if (currentRead > toRead)
		throw std::domain_error("unexpected read bigger than toRead");
	if (currentRead == toRead)
	{
		this->buffered.insert(
			this->buffered.end(),
			readBuffer,
			readBuffer + toRead);
		char *message = new char[this->buffered.size() + 1];
		::memcpy(message, &this->buffered[0], this->buffered.size());
		message[this->buffered.size()] = '\0';
		this->buffered.clear();
		this->readBefore = 0;
		return std::make_pair(DONE, message);
	}
	else // if (currentRead < toRead)
	{
		this->buffered.insert(
			this->buffered.end(),
			readBuffer,
			readBuffer + currentRead);
		this->readBefore += currentRead;
		return std::make_pair(READING, reinterpret_cast<char *>(0));
	}
}
