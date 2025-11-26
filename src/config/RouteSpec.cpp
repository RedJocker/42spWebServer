// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteSpec.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 11:29:09 by maurodri          #+#    #+#             //
//   Updated: 2025/11/25 21:47:46 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "RouteSpec.hpp"
#include "RouteCgi.hpp"
#include "RouteStaticFile.hpp"
#include "constants.hpp"
#include "Route.hpp"

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
		// TODO validate arguments
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
		// TODO validate arguments
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
}
