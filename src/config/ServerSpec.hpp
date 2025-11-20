// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ServerSpec.hpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/09 10:28:58 by maurodri          #+#    #+#             //
//   Updated: 2025/11/20 09:38:01 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef SERVERSPEC_HPP
# define SERVERSPEC_HPP

# include "VirtualServerSpec.hpp"
# include "pathUtils.hpp"
# include <string>
# include <vector>

namespace http
{
	class Server;
}

namespace config {

	class ServerSpec
	{
		std::string addressPort;
		std::string docroot;
		ssize_t maxSizeBody;
		bool listDirectories;
		std::string indexFile;
		MapErrorPages errorPages;
		std::vector<VirtualServerSpec> virtualServers;

	public:
		ServerSpec();
		ServerSpec(const ServerSpec &other);
		ServerSpec &operator=(const ServerSpec &other);
		virtual ~ServerSpec(void);

		const std::string &getDocroot(void) const;
		const std::string &getAddressPort(void) const;
		const MapErrorPages &getErrorPages(void) const;

		ServerSpec &setDocroot(const std::string &docroot);
		ServerSpec &setAddressPort(const std::string &addressPort);
		ServerSpec &setMaxSizeBody(const ssize_t maxSizeBody);
		ServerSpec &setListDirectories(bool listDirectories);
		ServerSpec &setIndexFile(const std::string &indexFile);
		ServerSpec &addErrorPage(
			unsigned short int status, const std::string &bodyPage);

		ServerSpec &addVirtualServer(VirtualServerSpec &virtualServer);

		http::Server *toServer(void);
	};
}

#endif
