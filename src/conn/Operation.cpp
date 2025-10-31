// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Operation.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/01 22:06:49 by maurodri          #+#    #+#             //
//   Updated: 2025/10/30 21:56:19 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Operation.hpp"


bool Operation::operator<(const Operation &other) const
{
	return this->fd < other.fd;
}

Operation Operation::declare(Operation::Type type, int fd, time_t timeLimit)
{
	return (Operation){type, fd, time(static_cast<time_t*>(0)), timeLimit, ""};
}


time_t Operation::timeToExpire(void) const
{
	time_t now = time(static_cast<time_t*>(0));
	return (this->timeInitOp + this->timeLimit) - now;
}
