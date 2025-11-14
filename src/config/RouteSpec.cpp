// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteSpec.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 11:29:09 by maurodri          #+#    #+#             //
//   Updated: 2025/11/14 02:32:14 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "RouteSpec.hpp"
#include "RouteCgi.hpp"
#include "RouteStaticFile.hpp"
#include "constants.hpp"


namespace config
{
	RouteSpec::RouteSpec(void):
		isCgi(false),
		docroot(""), // default on ServerSpec
		pathSpec(DEFAULT_PATH_SPEC),
		uploadFolder(""), // default on VirtualServerSpec
		maxSizeBody(MAX_SIZE_BODY_UNLIMITED),
		listDirectories(false), // default on ServerSpec
		listDirectoriesWasSet(false),
		indexFile(""),
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
		this->isCgi = other.isCgi;
		this->docroot = other.docroot;
		this->pathSpec = other.pathSpec;
		this->uploadFolder = other.uploadFolder;
		this->maxSizeBody = other.maxSizeBody;
		this->listDirectories = other.listDirectories;
		this->listDirectoriesWasSet = other.listDirectoriesWasSet;
		this->indexFile = other.indexFile;
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
		if (this->indexFile.empty())
			this->indexFile = indexFile;
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

	const std::string &RouteSpec::getPathSpec(void) const
	{
		return this->pathSpec;
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

	bool RouteSpec::isCgiRoute(void) const
	{
		return this->isCgi;
	}

	RouteSpec &RouteSpec::setCgiRoute(void)
	{
		this->isCgi = true;
		return *this;
	}

	http::Route *RouteSpec::toRoute(void)
	{

		// TODO set maxBodySize on Routes and implement feature
		// TODO set listDirectories on Routes and implement feature (do not list if false)
		// TODO set indexFile on Routes and implement feature
		http::Route *route;
		if (this->isCgi)
		{
			route = new http::RouteCgi(this->pathSpec,
									   this->docroot,
									   this->allowedMethods);
		}
		else
		{
			route = new http::RouteStaticFile(this->pathSpec,
											  this->uploadFolder,
											  this->docroot,
											  this->allowedMethods);
		}
		return route;
	}

}
