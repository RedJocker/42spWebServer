/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   devUtil.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 16:45:28 by maurodri          #+#    #+#             */
/*   Updated: 2025/10/13 13:20:07 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEVUTIL_HPP
# define DEVUTIL_HPP

# include <string>

namespace utils {
	void TODO(std::string msg);
	
	// Guess MIME type from extension
	std::string guessMimeType(const std::string &filePath);
}

#endif
