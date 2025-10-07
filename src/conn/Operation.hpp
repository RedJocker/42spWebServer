// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Operation.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/01 22:05:59 by maurodri          #+#    #+#             //
//   Updated: 2025/10/01 22:23:53 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef OPERATION_HPP
# define OPERATION_HPP

struct Operation
{
	enum Type {ANY, FILE_READ, FILE_WRITE, CGI};
	Operation::Type type;
	int fd;

	bool operator<(const Operation &other) const;
};

#endif
