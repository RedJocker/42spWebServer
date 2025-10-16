/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 11:36:17 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/16 20:13:57 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATHUTILS_HPP
# define PATHUTILS_HPP

#include <string>

namespace utils {
	std::string urlDecode(const std::string &url);
	std::string toString(size_t n);
	std::string guessMimeType(const std::string &filePath);
}

#endif
