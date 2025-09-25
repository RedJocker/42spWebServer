/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:00:29 by vcarrara          #+#    #+#             */
/*   Updated: 2025/09/25 18:48:07 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include "../conn/TcpServer.hpp"
# include <string>
# include <vector>

namespace http {
	class Server : public conn::TcpServer {
		std::string hostname;
		std::string docroot;
		std::vector<std::string> cgiRoutes;


		public:
			Server(const std::string &hostname,
					const std::string &docroot,
					unsigned short port);
			Server(const Server &other);
			Server &operator=(const Server &other);
			virtual ~Server(void);

			std::string getHostname() const;
			std::string getDocroot() const;
			
			void addCgiRoute(const std::string &route);
			const std::vector<std::string> &getCgiRoutes() const;
	};
}

#endif
