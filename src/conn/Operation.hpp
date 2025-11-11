// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Operation.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/01 22:05:59 by maurodri          #+#    #+#             //
/*   Updated: 2025/11/05 19:51:02 by maurodri         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#ifndef OPERATION_HPP
# define OPERATION_HPP

# include <ctime>
# include <string>
# include "BufferedReader.hpp"
# include "BufferedWriter.hpp"

struct Operation
{

public:
	enum Type {ANY, FILE_READ, FILE_WRITE, CGI};
	Operation::Type type;
	int fd;
	time_t timeInitOp;
	time_t timeLimit;
	mutable std::string content;
	mutable BufferedReader *reader;
	mutable BufferedWriter *writer;
	pid_t cgiPid;
	bool operator<(const Operation &other) const;
	time_t timeToExpire(void) const;
	static Operation declare(Operation::Type type, int fd, time_t timeLimit);
	static Operation matcher(int fd);
};

#endif
