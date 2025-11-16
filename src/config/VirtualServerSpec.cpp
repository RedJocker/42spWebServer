// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   VirtualServerSpec.cpp                              :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 11:03:12 by maurodri          #+#    #+#             //
//   Updated: 2025/11/16 05:48:38 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "VirtualServerSpec.hpp"
#include "constants.hpp"
#include "VirtualServer.hpp"

namespace config {

	VirtualServerSpec::VirtualServerSpec(void) :
		hostname(DEFAULT_HOSTNAME),
		docroot(""), // default on ServerSpec
		uploadFolder(DEFAULT_UPLOAD_FOLDER),
		maxSizeBody(MAX_SIZE_BODY_UNLIMITED),
		listDirectories(false), // default on ServerSpec
		listDirectoriesWasSet(false),
		indexFile(""),
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
		if (this->indexFile.empty())
			this->indexFile = indexFile;
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

	http::VirtualServer VirtualServerSpec::toVirtualServer(void)
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
				.setListDirectoriesIfUnset(this->listDirectories)
				.setIndexFileIfEmpty(this->indexFile)
				.addErrorPagesIfUnset(this->errorPages)
				.setRedirectionIfUnset(this->redirection)
				.toRoute();
			_routes.push_back(route);
		}
		http::VirtualServer virtualServer(*this,_routes);
		return virtualServer;
	}
}
