/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:00:29 by vcarrara          #+#    #+#             */
//   Updated: 2025/11/24 16:04:12 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include "ServerSpec.hpp"
# include "TcpServer.hpp"
# include "Client.hpp"
# include "VirtualServer.hpp"
# include "pathUtils.hpp"
# include <string>
# include <set>

namespace http
{
    class Server : public conn::TcpServer
	{
		std::string docroot;
		std::vector<VirtualServer*> vservers;
		MapErrorPages errorPages;

	public:
		Server(void);
		Server(
			const config::ServerSpec &spec,
			std::vector<VirtualServer*> &virtualServers);
		Server(const Server &other);
		Server &operator=(const Server &other);
		virtual ~Server(void);

		const std::string &getDocroot() const;
		const MapErrorPages &getErrorPages(void) const;

		void serve(Client &client, conn::Monitor &monitor);
		void shutdown(void);
	};
}

#endif
