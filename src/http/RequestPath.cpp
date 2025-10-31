/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestPath.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:58:32 by vcarrara          #+#    #+#             */
//   Updated: 2025/10/30 21:44:31 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "RequestPath.hpp"
#include "pathUtils.hpp"
#include <vector>

RequestPath::RequestPath()
{

}

const std::string &RequestPath::getOriginalPath() const {
	return originalPath;
}

const std::string &RequestPath::getQueryString() const {
	return queryString;
}

const std::string &RequestPath::getOriginalPathNormalized() const {
	return originalPathNormalized;
}

const std::string &RequestPath::getPath() const {
	return path;
}

const std::string &RequestPath::getFilePath() const {
	return filePath;
}

const std::string &RequestPath::getExtension() const {
	return extension;
}

const std::string &RequestPath::getDocroot() const {
	return docroot;
}


bool RequestPath::isDirectory() const {
	return directory;
}

bool RequestPath::needsTrailingSlashRedirect() const {
	return redirectNeeded;
}

bool RequestPath::isFile() const {
	return _isFile;
}

void RequestPath::initRequestPath(
	const std::string &rawPath, const std::string &docroot)
{
	this->originalPath = rawPath;
	this->docroot = docroot;
	this->normalize();
	this->splitQueryFromPath();
	this->analyzePath();
}

void RequestPath::splitQueryFromPath()
{
	// Separate query string
	std::string::size_type qpos = this->originalPathNormalized.find('?');
	if (qpos != std::string::npos) {
		this->queryString = this->originalPathNormalized.substr(qpos + 1);
		this->path = this->originalPathNormalized.substr(0, qpos);
	} else {
		this->queryString = "";
		this->path = this->originalPathNormalized;
	}
}

void RequestPath::normalize()
{
	// Decode percent-encoded characters
	std::string path = utils::urlDecode(this->originalPath);

	if (path.empty())
		path = "/";


	// Collapse multiple slashes
	std::string collapsed;
	collapsed.reserve(path.size());
	for (size_t i = 0; i < path.size(); ++i) {
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
	for (size_t i = 0; i < collapsed.size(); ) {
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

	std::string sanitized = "/";
	for (size_t k = 0; k < segments.size(); ++k)
	{
		if (k != 0)
			sanitized += "/";
		sanitized += segments[k];
	}
	if (path[path.size() - 1] == '/')
		sanitized.push_back('/');
	this->originalPathNormalized = sanitized;
}

void RequestPath::analyzePath() {

	this->filePath = this->docroot + this->path;

	struct stat pathStat;
	if (stat(this->filePath.c_str(), &pathStat) == 0) {
		this->directory = S_ISDIR(pathStat.st_mode);
		this->_isFile = S_ISREG(pathStat.st_mode);
		this->redirectNeeded = this->directory ?
			this->path[this->path.size() - 1] != '/' : false;
	} else {
		this->directory = false;
		this->redirectNeeded = false;
		this->_isFile = false;
	}

	// check extension, file may not exist (ex: POST uploading file)
	if (this->directory)
	{
		this->extension = "";
	}
	else
	{
		size_t dotIndex = this->path.rfind('.');
		if (dotIndex == std::string::npos)
			this->extension = "";
		else
			this->extension = this->path.substr(dotIndex);
	}
}
