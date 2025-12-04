/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 11:36:17 by vcarrara          #+#    #+#             */
/*   Updated: 2025/12/04 19:27:34 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATHUTILS_HPP
# define PATHUTILS_HPP

# include <string>
# include <map>
# include <cstdlib>

typedef std::map<unsigned short int, std::string> MapErrorPages;

namespace utils {
	std::string urlDecode(const std::string &url);
	std::string toString(size_t n);
	std::string guessMimeType(const std::string &filePath);
	bool endsWith(const std::string &maybeHasSuffix, const std::string &suffix);
	size_t findLastFromEnd(char toFind, std::string toSearch, size_t leftLimit);
	std::string lowercaseCopy(const std::string &str);
	bool startsWith(
		const std::string &prefix, const std::string &maybeHasPrefix);
	std::string &trimInPlace(std::string &maybeHasLeadAndTrailingSpaces);
	std::string trimCopy(const std::string &maybeHasLeadAndTrailingSpaces);
	ssize_t findOneOf (
		const std::string &toSearch,
		size_t startPos,
		const std::string &charsToFind);
	bool isDirectiveSimple(
		const std::string &prefix,
		const std::string &directive,
		ssize_t &out_end,
		ssize_t &out_prefixSize);
	bool isDirectiveCompound(
		const std::string &prefix,
		const std::string &directive,
		std::string &out_param,
		std::string &out_directives);
}

#endif
