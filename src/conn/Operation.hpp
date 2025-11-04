// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Operation.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/01 22:05:59 by maurodri          #+#    #+#             //
//   Updated: 2025/10/30 22:04:45 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef OPERATION_HPP
# define OPERATION_HPP

# include <ctime>
# include <string>

struct Operation
{
	enum Type {ANY, FILE_READ, FILE_WRITE, CGI};
	Operation::Type type;
	int fd;
	time_t timeInitOp;
	time_t timeLimit;
	std::string content;
	bool operator<(const Operation &other) const;
	time_t timeToExpire(void) const;
	static Operation declare(Operation::Type type, int fd, time_t timeLimit);
};

#endif
