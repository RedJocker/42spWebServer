/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestPath.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:58:32 by vcarrara          #+#    #+#             */
/*   Updated: 2025/12/10 19:03:22 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestPath.hpp"
#include "pathUtils.hpp"
#include <vector>
#include <iostream>
#include <sys/stat.h>


RequestPath::RequestPath() :
	originalPath(""),
	originalPathNormalized(""),
	path(""),
	queryString(""),
	filePath(""),
	extension(""),
	directory(false),
	redirectNeeded(false),
	_isFile(false)
{

}

RequestPath::RequestPath(const RequestPath &other)
{
	*this = other;
}

RequestPath &RequestPath::operator=(const RequestPath &other)
{
	if (this == &other)
		return *this;

	this->originalPath = other.originalPath;
	this->originalPathNormalized = other.originalPathNormalized;
	this->path = other.path;
	this->queryString = other.queryString;
	this->filePath = other.filePath;
	this->extension = other.extension;
	this->directory = other.directory;
	this->redirectNeeded = other.redirectNeeded;
	this->_isFile = other._isFile;

	return *this;
}

RequestPath::~RequestPath()
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

bool RequestPath::isDirectory() const {
	return directory;
}

bool RequestPath::needsTrailingSlashRedirect() const {
	return redirectNeeded;
}

bool RequestPath::isFile() const {
	return _isFile;
}

bool RequestPath::matchesPathSpecification(
	const std::string &pathSpecification) const
{
	// TODO validade pathSpecification while parsing config
	// TODO make unit test for this method
	// "/**.ext" should only be used at end of spec, that is, any "/" or "*" after "/**" is invalid
	// "/**.ext" matches any file with ".ext" either at current folder or any subfolder
	// "/**" should only be used at end of spec, mathing all files on folder and subfolders
	// "**" that is not preceded by "/" or is not at end of spec is invalid
	// "*" on end of spec matches only at same level, that is, if path has no extra "/"
	// "*" on other position with match until first occurrence of next spec char on path

	const std::string &spec = pathSpecification;
	const size_t spec_len = spec.size();
	const size_t path_len = this->path.size();
	size_t spec_i = 0;
	size_t path_i = 0;

	bool isMatch = false;
	while (true)
	{
		//std::cout << spec_i << ":" << path_i << std::endl;
		if (spec_i == spec_len)
		{
			isMatch = path_i == path_len;
			break;
		}
		if (path_i == path_len)
		{
			isMatch = spec_i == spec_len - 1 && spec.at(spec_i) == '*';
			break;
		}
		if (spec_i == spec_len - 3 && spec.substr(spec_i) == "/**")
		{
			isMatch = this->path.at(path_i) == '/';
			break;
		}
		if (spec.substr(spec_i, 3) == "/**")
		{
			if (this->path.at(path_i) != '/')
			{
				isMatch = false;
				break;
			}
			// there is at least one '/' at path_i, npos return not possible
			size_t path_last_slash =
				utils::findLastFromEnd('/', this->path, path_i);
			
			std::string path_end = this->path.substr(path_last_slash + 1);
			std::string spec_suffix = spec.substr(spec_i + 3);

			isMatch = utils::endsWith(path_end, spec_suffix);
			break;
		}

		if (spec.at(spec_i) != '*')
		{
			if (spec.at(spec_i) != this->path.at(path_i))
			{
				isMatch = false;
				break;
			}
			else
			{
				++spec_i;
				++path_i;
				continue;
			}
		}
		if (spec.at(spec_i) == '*' && spec_i == spec_len - 1)
		{
			isMatch = this->path.find('/', path_i) == std::string::npos;
			break;
		}
		// at this point spec[spec_i] == '*' and not at end
		++spec_i;
		char nextSpecChar = spec.at(spec_i);
		path_i = this->path.find(nextSpecChar, path_i);
		if (path_i == std::string::npos)
		{
			isMatch = false;
			break;
		}
		// at this point spec[spec_i] == path[path_i]
		++spec_i;
		++path_i;
	}
	std::cout << spec << " -> " << this->path << (isMatch ? " [YES]" : " [NO]")
			  << std::endl;
	return isMatch;
}

void RequestPath::initRequestPath(const std::string &rawPath)
{
	this->originalPath = rawPath;
	this->normalize();
	this->splitQueryFromPath();
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
	if (path[path.size() - 1] == '/' && path.size() != 1)
		sanitized.push_back('/');
	this->originalPathNormalized = sanitized;
}

void RequestPath::analyzePath(const std::string &docroot) {

	this->filePath = docroot + this->path;

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
