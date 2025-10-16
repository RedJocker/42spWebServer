/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 11:38:22 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/16 19:32:39 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pathUtils.hpp"
#include <cctype>
#include <sstream>


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
}

