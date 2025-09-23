/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:00:29 by vcarrara          #+#    #+#             */
//   Updated: 2025/09/23 19:12:06 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include "../conn/TcpServer.hpp"
# include <string>

namespace http {
	class Server : public conn::TcpServer {
		std::string hostname;
		std::string docroot;


		public:
			Server(const std::string &hostname,
					const std::string &docroot,
					unsigned short port);
			Server(const Server &other);
			Server &operator=(const Server &other);
			virtual ~Server(void);

			std::string getHostname() const;
			std::string getDocroot() const;
	};
}

#endif
