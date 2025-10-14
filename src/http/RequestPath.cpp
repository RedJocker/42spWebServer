/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestPath.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:58:32 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/13 14:31:45 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestPath.hpp"
#include "pathUtils.hpp"
#include <iostream>

RequestPath::RequestPath(const std::string &docroot, const std::string &url)
	: originalPath(url), directory(false), redirectNeeded(false)
{
	analyzePath(docroot);
}

std::string RequestPath::getOriginalPath() const {
	return originalPath;
}

std::string RequestPath::getQueryString() const {
	return queryString;
}

std::string RequestPath::getNormalizedPath() const {
	return normalizedPath;
}

std::string RequestPath::getFullPath() const {
	return fullPath;
}

bool RequestPath::isDirectory() const {
	return directory;
}

bool RequestPath::needsTrailingSlashRedirect() const {
	return redirectNeeded;
}

void RequestPath::analyzePath(const std::string &docroot) {
	// Separate query string
	std::string::size_type qpos = originalPath.find('?');
	if (qpos != std::string::npos) {
		queryString = originalPath.substr(qpos + 1);
		normalizedPath = originalPath.substr(0, qpos);
	} else {
		queryString = "";
		normalizedPath = originalPath;
	}

	// Normalize URL and resolve relative paths
	if (!utils::normalizeUrlPath(docroot, normalizedPath, fullPath)) {
		fullPath = "";
		directory = false;
		redirectNeeded = false;
		return;
	}

	// Check if is dir
	struct stat pathStat;
	if (stat(fullPath.c_str(), &pathStat) == 0) {
		if (S_ISDIR(pathStat.st_mode)) {
			directory = true;
			// Check if ends with '/'
			if (!normalizedPath.empty() && normalizedPath[normalizedPath.size() - 1] != '/')
				redirectNeeded = true;
		} else {
			directory = false;
			redirectNeeded = false;
		}
	} else {
		directory = false;
		redirectNeeded = false;
	}
}
