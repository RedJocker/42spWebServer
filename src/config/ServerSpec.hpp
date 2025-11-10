// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ServerSpec.hpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 10:28:58 by maurodri          #+#    #+#             //
//   Updated: 2025/11/09 12:29:03 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef SERVERSPEC_HPP
# define SERVERSPEC_HPP

# include "VirtualServerSpec.hpp"
# include "Server.hpp"
# include <string>
# include <vector>


namespace config {

	class ServerSpec
	{
		std::string docroot;
		unsigned short port;
		std::vector<VirtualServerSpec> virtualServers;

	public:
		ServerSpec();
		ServerSpec(const ServerSpec &other);
		ServerSpec &operator=(const ServerSpec &other);
		virtual ~ServerSpec(void);

		const std::string &getDocroot(void) const;
		const unsigned short &getPort(void) const;

		ServerSpec &setDocroot(const std::string &docroot);
		ServerSpec &setPort(const unsigned short &port);
		ServerSpec &addVirtualServer(VirtualServerSpec &virtualServer);

		http::Server toServer(void);
	};
}

#endif
