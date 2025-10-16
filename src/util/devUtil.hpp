/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   devUtil.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 16:45:28 by maurodri          #+#    #+#             */
/*   Updated: 2025/10/15 18:48:40 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEVUTIL_HPP
# define DEVUTIL_HPP

# include <stdexcept>
# include <sstream>


#define TODO(message) do												\
{																		\
	std::stringstream	errorMessage;									\
																		\
	errorMessage << "TODO: " << message << std::endl					\
				 << __FILE__ << ":" << __LINE__ << ":0 " << std::endl;	\
	throw std::domain_error(errorMessage.str());						\
} while(1);

#endif
