/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 11:36:17 by vcarrara          #+#    #+#             */
//   Updated: 2025/11/04 19:44:40 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#ifndef PATHUTILS_HPP
# define PATHUTILS_HPP

#include <string>

namespace utils {
	std::string urlDecode(const std::string &url);
	std::string toString(size_t n);
	std::string guessMimeType(const std::string &filePath);
	std::string lowercaseCopy(const std::string &str);
	bool startsWith(
		const std::string &prefix, const std::string &maybeHasPrefix);
	std::string &trimInPlace(std::string &maybeHasLeadAndTrailingSpaces);
}

#endif
