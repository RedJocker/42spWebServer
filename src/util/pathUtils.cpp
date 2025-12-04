/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 11:38:22 by vcarrara          #+#    #+#             */
/*   Updated: 2025/12/04 19:36:05 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pathUtils.hpp"
#include <cctype>
#include <sstream>
#include <iostream>


namespace utils {
	int hexDigitValue(char c) {
		if (c >= '0' && c <= '9')
			return (c - '0');
		if (c >= 'a' && c <= 'f')
			return (10 + (c - 'a'));
		if (c >= 'A' && c <= 'F')
			return (10 + (c - 'A'));
		return -1;
	}


	std::string urlDecode(const std::string &url) {
		std::string out;
		out.reserve(url.size());
		for (std::string::size_type i = 0; i < url.size(); ++i) {
			char c = url[i];
			if (c == '%' && i + 2 < url.size()) {
				int hi = hexDigitValue(url[i + 1]);
				int lo = hexDigitValue(url[i + 2]);
				if (hi >= 0 && lo >= 0) {
					out.push_back(static_cast<char>(hi * 16 + lo));
					i += 2;
				} else {
					out.push_back(c);
				}
			} else {
				out.push_back(c);
			}
		}
		return out;
	}


	std::string toString(size_t n) {
		std::ostringstream oss;
		oss << n;
		return oss.str();
	}

	std::string guessMimeType(const std::string &filePath) {
		// Find last dot


		std::string::size_type dotPos = filePath.rfind('.');

		// may receive extension only
		std::string ext = dotPos == std::string::npos ?
			filePath
			: filePath.substr(dotPos + 1);

		// To lower
		for (std::string::size_type i = 0; i < ext.size(); ++i) {
			if (ext[i] >= 'A' && ext[i] <= 'Z')
				ext[i] += 'a' - 'A';
		}


		// Map extensions to MIME types
		if (ext == "html" || ext == "htm") return "text/html";
		if (ext == "css") return "text/css";
		if (ext == "js") return "application/javascript";
		if (ext == "json") return "application/json";
		if (ext == "txt") return "text/plain";
		if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
		if (ext == "png") return "image/png";
		if (ext == "gif") return "image/gif";
		if (ext == "svg") return "image/svg+xml";
		if (ext == "ico") return "image/x-icon";
		if (ext == "pdf") return "application/pdf";
		if (ext == "xml") return "application/xml";
		if (ext == "zip") return "application/zip";

		return "application/octet-stream"; // fallback
	}

	std::string lowercaseCopy(const std::string &str)
	{
		std::string lowercase = str;
		for (size_t i = 0; i < lowercase.size(); ++i)
			lowercase[i] = static_cast<char>(std::tolower(lowercase[i]));
		return str;
	}

	bool startsWith(const std::string &prefix, const std::string &maybeHasPrefix)
	{
		if (prefix.size() > maybeHasPrefix.size())
			return false;
		for (size_t i = 0; i < prefix.size(); ++i)
		{
			if (prefix.at(i) != maybeHasPrefix.at(i))
				return false;
		}
		return true;
	}

	// return same string from input to make possible chaining
	std::string &trimInPlace(std::string &maybeHasLeadAndTrailingSpaces)
	{
		std::string &str = maybeHasLeadAndTrailingSpaces;
		while (!str.empty() && std::isspace(str[0]))
			str.erase(0, 1);
		while (!str.empty() && std::isspace(str[str.size() - 1]))
			str.erase(str.size() - 1, 1);
		return str;
	}

	std::string trimCopy(const std::string &maybeHasLeadAndTrailingSpaces)
	{
		std::string str = maybeHasLeadAndTrailingSpaces;
		return trimInPlace(str);
	}

	bool endsWith(const std::string &maybeHasSuffix, const std::string &suffix)
	{
		if (maybeHasSuffix.size() < suffix.size())
				return false;
		size_t maybeHasSuffix_i = maybeHasSuffix.size() - 1;
		size_t suffix_i = suffix.size() - 1;

		while (suffix_i > 0
			   && maybeHasSuffix.at(maybeHasSuffix_i) == suffix.at(suffix_i))
		{
			--maybeHasSuffix_i;
			--suffix_i;
		}
		return	maybeHasSuffix.at(maybeHasSuffix_i) == suffix.at(suffix_i);
	}

	size_t findLastFromEnd(char toFind, std::string toSearch, size_t leftLimit)
	{
		size_t toSearchLen = toSearch.size() > 0 ? toSearch.size() - 1 : 0;
		while (toSearchLen > leftLimit
			   && toSearch.at(toSearchLen) != toFind)
		{
			--toSearchLen;
		}
		return toSearch.at(toSearchLen) == toFind ?
			toSearchLen : std::string::npos;
	}

	ssize_t findOneOf (
		const std::string &toSearch, size_t startPos, const std::string &charsToFind)
	{
		for (size_t i = startPos; i < toSearch.size(); ++i)
		{
			for (size_t j = 0; j < charsToFind.size(); ++j)
			{
				if (toSearch.at(i) == charsToFind.at(j))
					return static_cast<ssize_t>(i);
			}
		}
		return -1;	
	}

	bool isDirectiveSimple(
		const std::string &prefix,
		const std::string &directive,
		ssize_t &out_end,
		ssize_t &out_prefixSize)
	{
		if (!utils::startsWith(prefix, directive))
		{
			return false;
		}
		out_prefixSize = prefix.size();
		out_end = utils::findOneOf(directive, out_prefixSize, ";");
		return out_end != -1
			&& out_prefixSize != -1
			&& out_end > out_prefixSize;
	}

	bool isDirectiveCompound(
		const std::string &prefix,
		const std::string &directive,
		std::string &out_param,
		std::string &out_directives)
	{
		if (!utils::startsWith(prefix, directive))
		{
			return false;
		}
		ssize_t openBracket = utils::findOneOf(directive, prefix.size(), "{");
		ssize_t closeBracket = utils::findLastFromEnd('}', directive, openBracket);
		if (openBracket == -1 || closeBracket == -1)
		{
			return false;
		}
		out_param = utils::trimCopy(
			directive.substr(prefix.size(), openBracket - prefix.size()));
		
		out_directives = utils::trimCopy(
			directive.substr(openBracket + 1, closeBracket - (openBracket + 1)));
		
		return true;
	}
}
