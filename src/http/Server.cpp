/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:05:25 by vcarrara          #+#    #+#             */
/*   Updated: 2025/09/25 18:47:49 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

namespace http {
	Server::Server(const std::string &hostname,
			const std::string &docroot,
			unsigned short port)
		: conn::TcpServer(port), hostname(hostname), docroot(docroot) {}

	Server::Server(const Server &other) : conn::TcpServer(other.port) {
		*this = other;
	}

	Server &Server::operator=(const Server &other) {
		if (this != &other) {
			this->hostname = other.hostname;
			this->docroot = other.docroot;
			this->port = other.port;
		}
		return *this;
	}

	Server::~Server(void) {}

	std::string Server::getHostname() const {
		return this->hostname;
	}

	std::string Server::getDocroot() const {
		return this->docroot;
	}
	
	void Server::addCgiRoute(const std::string &route) {
		for (std::vector<std::string>::const_iterator it = cgiRoutes.begin(); it != cgiRoutes.end(); ++it) {
			if (*it == route)
				return;
		}
		cgiRoutes.push_back(route);
	}
	
	const std::vector<std::string> &Server::getCgiRoutes() const {
		return cgiRoutes;
	}
}
