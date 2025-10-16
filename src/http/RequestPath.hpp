/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestPath.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:54:53 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/16 18:47:16 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPATH_HPP
#define REQUESTPATH_HPP

#include <string>
#include <sys/stat.h>

class RequestPath
{
	public:
		RequestPath();

		const std::string &getOriginalPath() const;
		const std::string &getQueryString() const;
		const std::string &getOriginalPathNormalized() const;
		const std::string &getPath() const;
		const std::string &getFilePath() const;
		const std::string &getExtension() const;
		std::string resourceMimeType() const;
		bool isDirectory() const;
		bool isFile() const;
		bool needsTrailingSlashRedirect() const;
		void initRequestPath(const std::string &rawPath, const std::string &docroot);

	private:
		std::string originalPath;   // from request
		std::string originalPathNormalized; // original normalized
		std::string path;    // normalized just path part, must have / on start
		std::string queryString;   // normalized just query part

		std::string filePath; // docroot + path
		std::string extension; // just extension of normalized
		std::string docroot; //  server docroot, does not have / on end
		bool directory;
		bool redirectNeeded;
		bool _isFile;
		void analyzePath();
		void splitQueryFromPath();
		void normalize();
};

#endif
