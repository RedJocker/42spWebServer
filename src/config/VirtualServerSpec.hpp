// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   VirtualServerSpec.hpp                              :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 11:03:46 by maurodri          #+#    #+#             //
//   Updated: 2025/11/10 00:19:58 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef VIRTUALSERVERSPEC_HPP
# define VIRTUALSERVERSPEC_HPP

# include <string>
# include <vector>
# include "RouteSpec.hpp"
# include "VirtualServer.hpp"

namespace config {

	class VirtualServerSpec
	{
		std::string hostname;
		std::string docroot;
		std::vector<RouteSpec> routes;

	public:
		VirtualServerSpec(void);
		VirtualServerSpec(const VirtualServerSpec &other);
		VirtualServerSpec &operator=(const VirtualServerSpec &other);
		virtual ~VirtualServerSpec(void);

		const std::string &getDocroot(void) const;

		VirtualServerSpec &setHostname(const std::string &hostname);
		VirtualServerSpec &setDocroot(const std::string &docroot);
		VirtualServerSpec &setDocrootIfEmpty(const std::string &docroot);
		VirtualServerSpec &addRoute(RouteSpec &route);
		http::VirtualServer toVirtualServer(void);
	};
}

#endif
