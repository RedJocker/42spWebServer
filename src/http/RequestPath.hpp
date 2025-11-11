/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestPath.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:54:53 by vcarrara          #+#    #+#             */
//   Updated: 2025/11/08 02:17:30 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPATH_HPP
#define REQUESTPATH_HPP

#include <string>

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
		const std::string &getDocroot() const;

		std::string resourceMimeType() const;
		bool isDirectory() const;
		bool isFile() const;
		bool needsTrailingSlashRedirect() const;
		void initRequestPath(const std::string &rawPath);
		bool matchesPathSpecification(
			const std::string &pathSpecification) const;
		void analyzePath(const std::string &docroot);
	private:
		//// set on initRequestPath
			std::string originalPath;   // from request
			std::string originalPathNormalized; // original normalized
			std::string path;    // normalized just path part, must have / on start
			std::string queryString;   // normalized just query part
		//// set on initRequestPath

		//// set on analyzePath
			std::string filePath; // docroot + path
			std::string extension; // just extension of normalized
			bool directory;
			bool redirectNeeded;
			bool _isFile;
		//// set on analyzePath

		void splitQueryFromPath();
		void normalize();
};

#endif
