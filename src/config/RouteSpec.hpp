// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteSpec.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 11:24:17 by maurodri          #+#    #+#             //
//   Updated: 2025/11/20 09:37:44 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //


#ifndef ROUTESPEC_HPP
# define ROUTESPEC_HPP

# include "pathUtils.hpp"
# include <string>
# include <vector>
# include <map>
# include <cstdlib>
# include <unistd.h>

namespace http
{
	class Route;
}

namespace config
{
	class RouteSpec
	{
		std::string cgiBinPath;
		std::string docroot;
		std::string pathSpec;
		std::string uploadFolder;
		ssize_t maxSizeBody;
		bool listDirectories;
		bool listDirectoriesWasSet;
		std::string indexFile;
		std::pair<unsigned short int, std::string> redirection;
		MapErrorPages errorPages;
		std::vector<std::string> allowedMethods;

	public:
		RouteSpec(void);
		RouteSpec(const RouteSpec &other);
		RouteSpec &operator=(const RouteSpec &other);
		virtual ~RouteSpec(void);

		const std::string &getDocroot(void) const;
		bool isCgiRoute(void) const;
		const std::string &getCgiBinPath(void) const;
		const std::string &getPathSpec(void) const;
		const std::vector<std::string> &getAllowedMethods(void) const;
		const std::string &getUploadFolder(void) const;
		const MapErrorPages &getErrorPages(void) const;

		RouteSpec &setCgiBinPath(const std::string &cgiBinPath);
		RouteSpec &setDocroot(const std::string &docroot);
		RouteSpec &setDocrootIfEmpty(const std::string &docroot);
		RouteSpec &setPathSpec(const std::string &pathSpec);
		RouteSpec &setUploadFolder(const std::string &uploadFolder);
		RouteSpec &setUploadFolderIfEmpty(const std::string &uploadFolder);
		RouteSpec &setMaxSizeBody(const ssize_t &maxSizeBody);
		RouteSpec &setMaxSizeBodyIfUnset(const ssize_t &maxSizeBody);
		RouteSpec &setListDirectories(bool listDirectories);
		RouteSpec &setListDirectoriesIfUnset(bool listDirectories);
		RouteSpec &setIndexFile(const std::string &indexFile);
		RouteSpec &setIndexFileIfEmpty(const std::string &indexFile);
		RouteSpec &setRedirection(
			unsigned short int statusCode, const std::string &path);
		RouteSpec &setRedirectionIfUnset(
			std::pair<unsigned short int, std::string> &redirection);
		RouteSpec &addAllowedMethod(const std::string &method);
		RouteSpec &addErrorPage(
			unsigned short int status, const std::string &bodyPage);
		RouteSpec &addErrorPagesIfUnset(
			const std::map<unsigned short int, std::string> pages);

		http::Route *toRoute(void);
	};
}

#endif
