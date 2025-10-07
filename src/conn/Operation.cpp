// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Operation.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/01 22:06:49 by maurodri          #+#    #+#             //
//   Updated: 2025/10/01 22:10:14 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Operation.hpp"

bool Operation::operator<(const Operation &other) const
{
	return this->fd < other.fd;
}
