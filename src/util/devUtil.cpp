// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   devUtil.cpp                                        :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/09/16 16:49:10 by maurodri          #+#    #+#             //
//   Updated: 2025/09/16 16:52:03 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "devUtil.hpp"
#include <stdexcept>

void TODO(std::string message)
{
	std::string errorMessage = "TODO: " + message;
	throw std::domain_error(errorMessage);
}
