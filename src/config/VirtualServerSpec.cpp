/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerSpec.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 11:03:12 by maurodri          #+#    #+#             */
/*   Updated: 2025/12/08 16:14:19 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualServerSpec.hpp"
#include "constants.hpp"
#include "VirtualServer.hpp"
#include <iostream>
#include "Scanner.hpp"
#include <sstream>

namespace config {

	VirtualServerSpec::VirtualServerSpec(void) :
		hostname(DEFAULT_HOSTNAME),
		docroot(""), // default on ServerSpec
		uploadFolder(DEFAULT_UPLOAD_FOLDER),
		maxSizeBody(MAX_SIZE_BODY_UNLIMITED),
		listDirectories(false), // default on ServerSpec
		listDirectoriesWasSet(false),
		indexFile(""),
		cgiTimeout(CGI_TIMEOUT_NONE),
		redirection(std::make_pair(0, "")),
		errorPages(),
		routes()
	{

	}

	VirtualServerSpec::VirtualServerSpec(const VirtualServerSpec &other)
		: maxSizeBody(other.maxSizeBody)
	{
		*this = other;
	}

	VirtualServerSpec &VirtualServerSpec::operator=(
		const VirtualServerSpec &other)
	{
		if (this == &other)
			return *this;
		this->hostname = other.hostname;
		this->docroot = other.docroot;
		this->routes = other.routes;
		this->uploadFolder = other.uploadFolder;
		this->maxSizeBody = other.maxSizeBody;
		this->listDirectories = other.listDirectories;
		this->listDirectoriesWasSet = other.listDirectoriesWasSet;
		this->indexFile = other.indexFile;
		this->cgiTimeout = other.cgiTimeout;
		this->errorPages = other.errorPages;
		this->redirection = other.redirection;
		return *this;
	}

	VirtualServerSpec::~VirtualServerSpec(void)
	{

	}

	const std::string &VirtualServerSpec::getDocroot(void) const
	{
		return this->docroot;
	}

	const std::string &VirtualServerSpec::getHostname(void) const
	{
		return this->hostname;
	}

	const MapErrorPages &VirtualServerSpec::getErrorPages(void) const
	{
		return this->errorPages;
	}

	const std::pair<unsigned short int, std::string>
		&VirtualServerSpec::getRedirection(void) const
	{
		return this->redirection;
	}

	VirtualServerSpec &VirtualServerSpec::setHostname(
		const std::string &hostname)
	{
		this->hostname = hostname;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setDocroot(
		const std::string &docroot)
	{
		this->docroot = docroot;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setDocrootIfEmpty(
		const std::string &docroot)
	{
		if (this->docroot.empty())
			this->docroot = docroot;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setUploadFolder(
		const std::string &uploadFolder)
	{
		this->uploadFolder = uploadFolder;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setMaxSizeBody(
		const ssize_t &maxSizeBody)
	{
		this->maxSizeBody = maxSizeBody;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setMaxSizeBodyIfUnset(
		const ssize_t &maxSizeBody)
	{
		if (this->maxSizeBody < 0)
			this->maxSizeBody = maxSizeBody;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::addRoute(RouteSpec &route)
	{
		this->routes.push_back(route);
		this->routes.back().setDocrootIfEmpty(this->docroot);
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setListDirectories(
		bool listDirectories)
	{
		this->listDirectories = listDirectories;
		this->listDirectoriesWasSet = true;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setListDirectoriesIfUnset(
		bool listDirectories)
	{
		if (this->listDirectoriesWasSet == false)
			this->setListDirectories(listDirectories);
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setIndexFile(
		const std::string &indexFile)
	{
		this->indexFile = indexFile;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setIndexFileIfEmpty(
		const std::string &indexFile)
	{
		if (this->indexFile.empty() && !this->listDirectoriesWasSet)
			this->indexFile = indexFile;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setCgiTimeout(time_t cgiTimeout)
	{
		this->cgiTimeout = cgiTimeout;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setCgiTimeoutIfUnset(time_t cgiTimeout)
	{
		if (this->cgiTimeout == CGI_TIMEOUT_NONE)
			this->cgiTimeout = cgiTimeout;
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::setRedirection(
		unsigned short int statusCode, const std::string &path)
	{
		// TODO validate arguments
		this->redirection = std::make_pair(statusCode, path);
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::addErrorPage(
		unsigned short int status, const std::string &bodyPage)
	{
		this->errorPages.insert(std::make_pair(status, bodyPage));
		return *this;
	}

	VirtualServerSpec &VirtualServerSpec::addErrorPagesIfUnset(
		const std::map<unsigned short int, std::string> pages)
	{
		// documentations states insert only inserts if key does not exist
		this->errorPages.insert(pages.begin(), pages.end());
		return *this;
	}

	http::VirtualServer *VirtualServerSpec::toVirtualServer(void)
	{
		std::vector<http::Route*> _routes;

		for (std::vector<RouteSpec>::iterator routeIt
				 = this->routes.begin();
			 routeIt != this->routes.end();
			 ++routeIt)
		{
			http::Route *route = (*routeIt)
				.setUploadFolderIfEmpty(this->uploadFolder)
				.setDocrootIfEmpty(this->docroot)
				.setMaxSizeBodyIfUnset(this->maxSizeBody)
				.setIndexFileIfEmpty(this->indexFile)
				.setCgiTimeoutIfUnset(this->cgiTimeout)
				.setListDirectoriesIfUnset(this->listDirectories)
				.addErrorPagesIfUnset(this->errorPages)
				.setRedirectionIfUnset(this->redirection)
				.toRoute();
			_routes.push_back(route);
		}
		http::VirtualServer *virtualServer = new http::VirtualServer(*this,_routes);
		return virtualServer;
	}

	int VirtualServerSpec::interpretDirective(const std::string &directive, Scanner &scanner)
	{ // TODO handle errors
		ssize_t end;
		ssize_t prefixSize;

		if (utils::isDirectiveSimple("server_name", directive, end, prefixSize))
		{
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			std::cout << "Setting Hostname to: " << value << std::endl;
			this->setHostname(value);
			return 0;
		}
		if (utils::isDirectiveSimple("root", directive, end, prefixSize))
		{
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			std::cout << "Setting docroot to: " << value << std::endl;
			this->setDocroot(value);
			return 0;
		}
		if (utils::isDirectiveSimple("upload_pass", directive, end, prefixSize))
		{
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
			std::cout << "Setting uploadFolder to: " << value << std::endl;
			this->setUploadFolder(value);
			return 0;
		}
		if (utils::isDirectiveSimple("index", directive, end, prefixSize))
		{
			std::string value = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
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
			std::cout << "Setting Max body size to: " << value << std::endl;
			this->setMaxSizeBody(value);
			return 0;
		}
		if (utils::isDirectiveSimple("autoindex", directive, end, prefixSize))
		{
			std::string valueStr = utils::trimCopy(
				directive.substr(prefixSize, end - prefixSize));
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
			size_t value;
			ss >> value;
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
			std::string content = utils::readErrorPage(path);
			std::cout << "Setting error pages to: " << status << ", " 
				<< content << std::endl;
			// TODO read file and send content to addErrorPage
			this->addErrorPage(status, content);
			return 0;
		}
		std::string param;
		std::string innerDirectives;
		if (utils::isDirectiveCompound("location", directive, param, innerDirectives))
		{
			RouteSpec routeSpec;
			if (routeSpec.routeConfigParse(innerDirectives, scanner) != 0)
			{
				std::cerr << "Error parsing location directive" << std::endl;
				return -1;
			}
			this->addRoute(routeSpec);
			return 0;
		}
		return 0;
	}

	int VirtualServerSpec::virtualServerConfigParse(
		const std::string &directivesStr, Scanner &scanner)
	{
		std::cout << "Parsing virtual server directive: " << directivesStr << std::endl;
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
			this->interpretDirective(this->directives[i], scanner);
		}
		return 0;
	}
}
