/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteSpec.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 11:29:09 by maurodri          #+#    #+#             */
/*   Updated: 2025/12/09 19:38:47 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RouteSpec.hpp"
#include "RouteCgi.hpp"
#include "RouteStaticFile.hpp"
#include "constants.hpp"
#include "Route.hpp"
#include <iostream>
#include <sstream>

namespace config
{
	RouteSpec::RouteSpec(void):
		cgiBinPath(""),
		docroot(""), // default on ServerSpec
		pathSpec(DEFAULT_PATH_SPEC),
		uploadFolder(""), // default on VirtualServerSpec
		maxSizeBody(MAX_SIZE_BODY_UNLIMITED),
		listDirectories(false), // default on ServerSpec
		listDirectoriesWasSet(false),
		indexFile(""),
		cgiTimeout(CGI_TIMEOUT_NONE),
		redirection(std::make_pair(0, "")),
		errorPages(),
		allowedMethods()
	{
	}

	RouteSpec::RouteSpec(const RouteSpec &other)
		: maxSizeBody(MAX_SIZE_BODY_UNLIMITED)
	{
		*this = other;
	}

	RouteSpec &RouteSpec::operator=(const RouteSpec &other)
	{
		if (this == &other)
			return *this;
		this->cgiBinPath = other.cgiBinPath;
		this->docroot = other.docroot;
		this->pathSpec = other.pathSpec;
		this->uploadFolder = other.uploadFolder;
		this->maxSizeBody = other.maxSizeBody;
		this->listDirectories = other.listDirectories;
		this->listDirectoriesWasSet = other.listDirectoriesWasSet;
		this->indexFile = other.indexFile;
		this->cgiTimeout = other.cgiTimeout;
		this->redirection = other.redirection;
		this->errorPages = other.errorPages;
		this->allowedMethods = other.allowedMethods;
		return *this;
	}

	RouteSpec::~RouteSpec(void)
	{

	}

	const std::string &RouteSpec::getDocroot(void) const
	{
		return this->docroot;
	}

	const std::string &RouteSpec::getCgiBinPath(void) const
	{
		return this->cgiBinPath;
	}

	const std::string &RouteSpec::getPathSpec(void) const
	{
		return this->pathSpec;
	}

	const std::vector<std::string> &RouteSpec::getAllowedMethods(void) const
	{
		return this->allowedMethods;
	}

	const std::string &RouteSpec::getUploadFolder(void) const
	{
		return this->uploadFolder;
	}

	const std::map<unsigned short int, std::string> &RouteSpec::getErrorPages(
		void) const
	{
		return this->errorPages;
	}

	const std::string &RouteSpec::getIndexFile(void) const
	{
		return this->indexFile;
	}

	bool RouteSpec::getListDirectories(void) const
	{
		return this->listDirectories && this->indexFile.empty();
	}

	const std::pair<unsigned short int, std::string>
		&RouteSpec::getRedirection(void) const
	{
		return this->redirection;
	}

	time_t RouteSpec::getCgiTimeout(void) const
	{
		return this->cgiTimeout;
	}

	ssize_t RouteSpec::getMaxSizeBody(void) const
	{
		return this->maxSizeBody;
	}

	bool RouteSpec::isCgiRoute(void) const
	{
		return !this->cgiBinPath.empty();
	}

	RouteSpec &RouteSpec::setDocroot(const std::string &docroot)
	{
		this->docroot = docroot;
		return *this;
	}

	RouteSpec &RouteSpec::setDocrootIfEmpty(const std::string &docroot)
	{
		if (this->docroot.empty())
			this->docroot = docroot;
		return *this;
	}

	RouteSpec &RouteSpec::setUploadFolder(const std::string &uploadFolder)
	{
		this->uploadFolder = uploadFolder;
		return *this;
	}

	RouteSpec &RouteSpec::setUploadFolderIfEmpty(const std::string &uploadFolder)
	{
		if (this->uploadFolder.empty())
			this->uploadFolder = uploadFolder;
		return *this;
	}

	RouteSpec &RouteSpec::setMaxSizeBody(const ssize_t &maxSizeBody)
	{
		this->maxSizeBody = maxSizeBody;
		return *this;
	}

	RouteSpec &RouteSpec::setMaxSizeBodyIfUnset(const ssize_t &maxSizeBody)
	{
		if (this->maxSizeBody < 0)
			this->maxSizeBody = maxSizeBody;
		return *this;
	}

	RouteSpec &RouteSpec::setListDirectories(bool listDirectories)
	{
		this->listDirectories = listDirectories;
		this->listDirectoriesWasSet = true;
		return *this;
	}

	RouteSpec &RouteSpec::setListDirectoriesIfUnset(bool listDirectories)
	{
		if (this->listDirectoriesWasSet == false)
			this->setListDirectories(listDirectories);
		return *this;
	}

	RouteSpec &RouteSpec::setIndexFile(const std::string &indexFile)
	{
		this->indexFile = indexFile;
		return *this;
	}

	RouteSpec &RouteSpec::setIndexFileIfEmpty(const std::string &indexFile)
	{
		if (this->indexFile.empty() && !this->listDirectoriesWasSet)
			this->indexFile = indexFile;
		return *this;
	}

	RouteSpec &RouteSpec::setCgiTimeout(time_t cgiTimeout)
	{
		this->cgiTimeout = cgiTimeout;
		return *this;
	}

	RouteSpec &RouteSpec::setCgiTimeoutIfUnset(time_t cgiTimeout)
	{
		if (this->cgiTimeout == CGI_TIMEOUT_NONE)
			this->cgiTimeout = cgiTimeout;
		return *this;
	}

	RouteSpec &RouteSpec::setRedirection(
		unsigned short int statusCode, const std::string &path)
	{
		this->redirection = std::make_pair(statusCode, path);
		return *this;
	}

	RouteSpec &RouteSpec::setRedirectionIfUnset(
		std::pair<unsigned short int, std::string> &redirection)
	{
		if (this->redirection.first != 0)
		{
			return *this;
		}
		this->redirection = redirection;
		return *this;
	}

	RouteSpec &RouteSpec::addErrorPage(
		unsigned short int status, const std::string &bodyPage)
	{
		this->errorPages.insert(std::make_pair(status, bodyPage));
		return *this;
	}
	RouteSpec &RouteSpec::addErrorPagesIfUnset(
		const std::map<unsigned short int, std::string> pages)
	{
		// documentations states insert only inserts if key does not exist
		this->errorPages.insert(pages.begin(), pages.end());
		return *this;
	}

	RouteSpec &RouteSpec::setPathSpec(const std::string &pathSpec)
	{
		this->pathSpec = pathSpec;
		return *this;
	}

	RouteSpec &RouteSpec::addAllowedMethod(const std::string &method)
	{
		this->allowedMethods.push_back(method);
		return *this;
	}

	RouteSpec &RouteSpec::setCgiBinPath(const std::string &cgiBinPath)
	{
		this->cgiBinPath = cgiBinPath;
		return *this;
	}

	http::Route *RouteSpec::toRoute(void)
	{
		http::Route *route;
		if (this->isCgiRoute())
		{
			route = new http::RouteCgi(*this);
		}
		else
		{
			route = new http::RouteStaticFile(*this);
		}
		return route;
	}

	int RouteSpec::interpretDirective(const std::string &directive, Scanner &scanner)
	{
		ssize_t end;
		ssize_t prefixSize;

		if (utils::isDirectiveSimple("cgi_bin", directive, end, prefixSize))
		{ 
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			if (!utils::fileisExecutable(value))
			{
				std::cerr << "cgi_bin file is not executable: " << value << std::endl;
				return -1;
			}
			std::cout << "Setting cgiBinPath to: " << value << std::endl;
			this->setCgiBinPath(value);
			return 0;
		}
		if (utils::isDirectiveSimple("root", directive, end, prefixSize))
		{
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			if(!utils::fileisDirectory(value))
			{
				std::cerr << "root is not Directory: " << value << std::endl;
				return -1;				
			}
			std::cout << "Setting docroot to: " << value << std::endl;
			this->setDocroot(value);
			return 0;
		}
		if (utils::isDirectiveSimple("upload_pass", directive, end, prefixSize))
		{
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			if(!utils::fileisDirectory(value))
			{
				std::cerr << "upload_pass is not Directory: " << value << std::endl;
				return -1;				
			}
			std::cout << "Setting uploadFolder to: " << value << std::endl;
			this->setUploadFolder(value);
			return 0;
		}
		if (utils::isDirectiveSimple("index", directive, end, prefixSize))
		{
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			if(utils::isBlank(value))
			{
				std::cerr << "index is blank: " << value << std::endl;
				return -1;
			}
			std::cout << "Setting index file to: " << value << std::endl;
			this->setIndexFile(value);
			return 0;
		}
		if (utils::isDirectiveSimple("client_max_body_size", directive, end, prefixSize))
		{
			std::string valueStr = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			std::stringstream ss(valueStr);
			ssize_t value;
			ss >> value;
			if(ss.fail())
			{
				std::cerr << "Failed to parse directive:" << directive << std::endl;
				return -1;
			}
			if(value < 0)
			{
				std::cerr << "Failed to parse: client_max_body_size < 0" << std::endl;
				return -1;
			}
			std::cout << "Setting Max body size to: " << value << std::endl;
			this->setMaxSizeBody(value);
			return 0;
		}
		if (utils::isDirectiveSimple("autoindex", directive, end, prefixSize))
		{
			std::string valueStr = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			if (!(valueStr == "on" || valueStr == "true" || valueStr == "off" || valueStr == "false")) 
			{
				std::cerr << "Invalid value for autoindex directive: " << valueStr << std::endl;
				return -1;
			}
			bool value = (valueStr == "on" || valueStr == "true");
			std::cout << "Setting list directories to: " << value << std::endl;
			this->setListDirectories(value);
			return 0;
		}
		if (utils::isDirectiveSimple("fastcgi_read_timeout", directive, end, prefixSize))
		{
			std::string valueStr = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			std::stringstream ss(valueStr);
			ssize_t value;
			ss >> value;
			if(ss.fail())
			{
				std::cerr << "Failed to parse directive:" << directive << std::endl;
				return -1;
			}
			if(value < 0)
			{
				std::cerr << "Failed to parse: fastcgi_read_timeout < 0" << std::endl;
				return -1;
			}
			std::cout << "Setting cgiTimeout to: " << value << std::endl;
			this->setCgiTimeout(value);
			return 0;
		}
		if (utils::isDirectiveSimple("return", directive, end, prefixSize))
		{
			std::string valueStr = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			std::stringstream ss(valueStr);
			std::string path;
			unsigned short status;
			ss >> status >> path;
			if(ss.fail())
			{
				std::cerr << "Failed to parse directive:" << directive << std::endl;
				return -1;
			}
			if(status < 300 || status >= 400)
			{
				std::cerr << "Failed to parse: return not in range 300..399" << std::endl;
				return -1;
			}
			std::cout << "Setting redirection to: " << status << ", " 
				<< path << std::endl;
			this->setRedirection(status, path);
			return 0;
		}
		if (utils::isDirectiveSimple("error_page", directive, end, prefixSize))
		{
			std::string valueStr = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			std::stringstream ss(valueStr);
			std::string path;
			size_t status;
			ss >> status >> path;
			std::string content;
			if(ss.fail())
			{
				std::cerr << "Failed to parse directive:" << directive << std::endl;
				return -1;
			}
			if(status < 400 || status >= 600)
			{
				std::cerr << "Failed to parse: error_page not in range 400..599" << std::endl;
				return -1;
			}
			if(utils::readErrorPage(path, content) != 0)
			{
				std::cerr << "Error reading error page file: " << path << std::endl;
				return -1;
			}
			std::cout << "Setting error pages to: " << status << ", " 
				<< content << std::endl;
			this->addErrorPage(status, content);
			return 0;
		}
		if (utils::isDirectiveSimple("allow", directive, end, prefixSize))
		{
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			if(value != "POST" && value != "GET" && value != "DELETE")
			{
				std::cerr << "Invalid Method: " << value << std::endl;
				return -1;
			}
			std::cout << "Adding allowedMethod: " << value << std::endl;
			this->addAllowedMethod(value);
			return 0;
		}
		std::string param;
		std::string innerDirectives;
		(void)scanner;
		return 0;
	}
	
	int RouteSpec::routeConfigParse(
		const std::string &directivesStr, Scanner &scanner)
	{
		(void)scanner;
		ssize_t alreadyread = 0;
		while (alreadyread <static_cast<ssize_t>(directivesStr.size()))
		{
			alreadyread = scanner
				.readDirective(directivesStr, alreadyread, this->directives);
			if (alreadyread < 0)
			{
				std::cerr << "Error parsing directive" << std::endl;
				return -1;
			}
		}
		
		for (size_t i = 0; i < this->directives.size(); ++i)
		{
			std::cout << "Directive " << i << ": "
					  << this->directives[i] << std::endl;
			if(this->interpretDirective(this->directives[i], scanner) != 0)
			{
				std::cerr << "Error interpreting directive" << std::endl;
				return -1;
			}
		}
		return 0;
	}
}
