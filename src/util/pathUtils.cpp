/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 11:38:22 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/13 11:24:36 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pathUtils.hpp"
#include <vector>
#include <cctype>

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

	bool normalizeUrlPath(const std::string &docroot, const std::string &urlPath, std::string &outFullPath) {
		// Strip query string
		std::string path = urlPath;
		std::string::size_type qpos = path.find('?');
		if (qpos != std::string::npos)
			path = path.substr(0, qpos);
		if (path.empty())
			path = "/";

		// Decode percent-encoded characters
		path = urlDecode(path);

		// Collapse multiple slashes
		std::string collapsed;
		collapsed.reserve(path.size());
		for (std::string::size_type i = 0; i < path.size(); ++i) {
			char c = path[i];
			if (c == '/') {
				if (collapsed.empty() || collapsed[collapsed.size() - 1] != '/')
					collapsed.push_back('/');
			} else {
				collapsed.push_back(c);
			}
		}

		// Split into segments and resolve . and ..
		std::vector<std::string> segments;
		for (std::string::size_type i = 0; i < collapsed.size(); ) {
			if (collapsed[i] == '/') {
				++i;
				continue;
			}
			std::string::size_type j = i;
			while (j < collapsed.size() && collapsed[j] != '/')
				++j;
			std::string seg = collapsed.substr(i, j - i);
			i = j;

			if (seg.empty() || seg == ".") {
				continue;
			} else if (seg == "..") {
				if (segments.empty())
					continue;
				segments.pop_back();
			} else {
				segments.push_back(seg);
			}
		}

		// Reconstruct sanitized path (leading '/')
		std::string sanitized = "/";
		for (std::vector<std::string>::size_type k = 0; k < segments.size(); ++k) {
			if (k != 0)
				sanitized += "/";
			sanitized += segments[k];
		}

		// Combine with docroot
		std::string root = docroot;
		if (!root.empty() && root[root.size() - 1] == '/')
			root.erase(root.size() - 1);

		if (sanitized == "/") {
			outFullPath = root + "/";
		} else {
			outFullPath = root + sanitized;
		}

		// Prevent docroot escape
		if (outFullPath.compare(0, root.size(), root) != 0) {
			return false;
		}

		return true;
	}
}
