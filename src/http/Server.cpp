/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:05:25 by vcarrara          #+#    #+#             */
/*   Updated: 2025/09/23 13:06:50 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

namespace http {
	Server::Server(const std::string &hostname,
			const std::string &docroot,
			unsigned short port)
		: hostname(hostname), docroot(docroot), port(port) {}

	Server::Server(const Server &other) {
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

	unsigned short Server::getPort() const {
		return this->port;
	}
}
