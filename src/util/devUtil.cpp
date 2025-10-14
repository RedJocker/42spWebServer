/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   devUtil.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 16:49:10 by maurodri          #+#    #+#             */
/*   Updated: 2025/10/13 13:23:46 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "devUtil.hpp"
#include <stdexcept>
#include <cstring>

namespace utils {
	void TODO(std::string message)
	{
		std::string errorMessage = "TODO: " + message;
		throw std::domain_error(errorMessage);
	}

	std::string guessMimeType(const std::string &filePath) {
		// Find last dot
		std::string::size_type dotPos = filePath.rfind('.');
		if (dotPos == std::string::npos)
			return "application/octet-stream";

		std::string ext = filePath.substr(dotPos + 1);

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
