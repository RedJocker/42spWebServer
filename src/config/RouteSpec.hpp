// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteSpec.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 11:24:17 by maurodri          #+#    #+#             //
//   Updated: 2025/11/12 17:27:15 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //


#ifndef ROUTESPEC_HPP
# define ROUTESPEC_HPP

# include "Route.hpp"
# include <string>
# include <vector>

namespace config
{
	class RouteSpec
	{
		bool isCgi;
		std::string docroot;
		std::string pathSpec;
		std::string uploadFolder;
		std::vector<std::string> allowedMethods;

	public:
		RouteSpec(void);
		RouteSpec(const RouteSpec &other);
		RouteSpec &operator=(const RouteSpec &other);
		virtual ~RouteSpec(void);

		const std::string &getDocroot(void) const;
		bool isCgiRoute(void) const;
		const std::string &getPathSpec(void) const;

		RouteSpec &setCgiRoute(void);
		RouteSpec &setDocroot(const std::string &docroot);
		RouteSpec &setDocrootIfEmpty(const std::string &docroot);
		RouteSpec &setPathSpec(const std::string &pathSpec);
		RouteSpec &addAllowedMethod(const std::string &method);
		RouteSpec &setUploadFolder(const std::string &uploadFolder);
		RouteSpec &setUploadFolderIfEmpty(const std::string &uploadFolder);

		http::Route *toRoute(void);
	};
}

#endif
