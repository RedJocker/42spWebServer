/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestPath.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:54:53 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/13 14:23:38 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPATH_HPP
#define REQUESTPATH_HPP

#include <string>
#include <sys/stat.h>
#include "pathUtils.hpp"

class RequestPath {
	public:
		RequestPath(const std::string &docroot, const std::string &url);

		std::string getOriginalPath() const;
		std::string getQueryString() const;
		std::string getNormalizedPath() const;
		std::string getFullPath() const;
		bool isDirectory() const;
		bool needsTrailingSlashRedirect() const;

	private:
		std::string originalPath;
		std::string queryString;
		std::string normalizedPath;
		std::string fullPath;
		bool directory;
		bool redirectNeeded;

		void analyzePath(const std::string &docroot);
};

#endif
