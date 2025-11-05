// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Operation.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/01 22:06:49 by maurodri          #+#    #+#             //
/*   Updated: 2025/11/05 20:19:26 by maurodri         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#include "Operation.hpp"


bool Operation::operator<(const Operation &other) const
{
	return this->fd < other.fd;
}

Operation Operation::declare(Operation::Type type, int fd, time_t timeLimit)
{
	BufferedReader *reader = reinterpret_cast<BufferedReader *>(0);
	BufferedWriter *writer = reinterpret_cast<BufferedWriter *>(0);
	time_t timeInit = 0;
	if (type == FILE_READ || type == CGI) {
		reader = new BufferedReader(fd);
	}
	if (type == FILE_WRITE || type == CGI) {
		writer = new BufferedWriter(fd);
	}
	if (type != ANY)
		timeInit = time(reinterpret_cast<time_t *>(0));

	return (Operation)
		{.type = type,
		 .fd = fd,
		 .timeInitOp = timeInit,
		 .timeLimit = timeLimit,
		 .content = "",
		 .reader = reader,
		 .writer = writer,
		 .cgiPid = -22
		};
}

Operation Operation::matcher(int fd)
{
	return (Operation)
		{.type = ANY,
		 .fd = fd,
		 .timeInitOp = 0,
		 .timeLimit = 0,
		 .content = "",
		 .reader = 0,
		 .writer = 0,
		 .cgiPid = -22
		};
}

time_t Operation::timeToExpire(void) const
{
	time_t now = time(static_cast<time_t*>(0));
	return (this->timeInitOp + this->timeLimit) - now;
}
