/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSpec.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 10:28:58 by maurodri          #+#    #+#             */
/*   Updated: 2025/12/09 20:04:38 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSPEC_HPP
# define SERVERSPEC_HPP

# include "VirtualServerSpec.hpp"
# include "pathUtils.hpp"
# include <string>
# include <vector>
# include "Scanner.hpp"

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
		time_t cgiTimeout;
		MapErrorPages errorPages;
		
		std::vector<VirtualServerSpec> virtualServers;

		std::vector<std::string> directives;

		int interpretDirective(const std::string &directive, Scanner &scanner);

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
		ServerSpec &setCgiTimeout(time_t cgiTimeout);
		ServerSpec &addErrorPage(
			unsigned short int status, const std::string &bodyPage);

		ServerSpec &addVirtualServer(VirtualServerSpec &virtualServer);

		http::Server *toServer(void);

		std::string toString(void) const;

		int serverConfigParse(const std::string &directive, Scanner &scanner);
	};
}

#endif
