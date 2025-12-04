/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerSpec.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 11:03:46 by maurodri          #+#    #+#             */
/*   Updated: 2025/12/04 19:50:06 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUALSERVERSPEC_HPP
# define VIRTUALSERVERSPEC_HPP

# include <string>
# include <vector>
# include "RouteSpec.hpp"
# include "pathUtils.hpp"
# include "Scanner.hpp"


namespace http
{
	class VirtualServer;
}


namespace config {

	class VirtualServerSpec
	{
		std::string hostname;
		std::string docroot;
		std::string uploadFolder;
		ssize_t maxSizeBody;
		bool listDirectories;
		bool listDirectoriesWasSet;
		std::string indexFile;
		time_t cgiTimeout;

		std::pair<unsigned short int, std::string> redirection;
		MapErrorPages errorPages;
		std::vector<RouteSpec> routes;

	public:
		VirtualServerSpec(void);
		VirtualServerSpec(const VirtualServerSpec &other);
		VirtualServerSpec &operator=(const VirtualServerSpec &other);
		virtual ~VirtualServerSpec(void);

		const std::string &getDocroot(void) const;
		const std::string &getHostname(void) const;
		const MapErrorPages &getErrorPages(void) const;
		const std::pair<unsigned short int, std::string>
			&getRedirection(void) const;

		VirtualServerSpec &setHostname(const std::string &hostname);
		VirtualServerSpec &setDocroot(const std::string &docroot);
		VirtualServerSpec &setDocrootIfEmpty(const std::string &docroot);
		VirtualServerSpec &setUploadFolder(const std::string &uploadFolder);
		VirtualServerSpec &setMaxSizeBody(const ssize_t &maxSizeBody);
		VirtualServerSpec &setMaxSizeBodyIfUnset(const ssize_t &maxSizeBody);
		VirtualServerSpec &setListDirectories(bool listDirectories);
		VirtualServerSpec &setListDirectoriesIfUnset(bool listDirectories);
		VirtualServerSpec &setIndexFile(const std::string &indexFile);
		VirtualServerSpec &setIndexFileIfEmpty(const std::string &indexFile);
		VirtualServerSpec &setCgiTimeout(time_t cgiTimeout);
		VirtualServerSpec &setCgiTimeoutIfUnset(time_t cgiTimeout);
		VirtualServerSpec &setRedirection(
			unsigned short int statusCode, const std::string &path);
		VirtualServerSpec &addErrorPage(
			unsigned short int status, const std::string &bodyPage);
		VirtualServerSpec &addErrorPagesIfUnset(
			const std::map<unsigned short int, std::string> pages);

		VirtualServerSpec &addRoute(RouteSpec &route);

		http::VirtualServer *toVirtualServer(void);
		int virtualServerConfigParse(const std::string &directives, Scanner &scanner);
	};
}

#endif
