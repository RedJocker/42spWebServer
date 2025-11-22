/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:41:30 by maurodri          #+#    #+#             */
//   Updated: 2025/11/20 06:30:46 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Dispatcher.hpp"
#include "Monitor.hpp"
#include "pathUtils.hpp"
#include "Server.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <vector>
#include <cerrno>
#include <cstring>
#include "devUtil.hpp"
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include "EventLoop.hpp"

namespace http {

	Dispatcher::Dispatcher() {}

	Dispatcher::Dispatcher(const Dispatcher &other)
	{
		(void) other;
	}

	Dispatcher &Dispatcher::operator=(const Dispatcher &other)
	{
		if (this == &other)
			return *this;

		return *this;
	}

	Dispatcher::~Dispatcher() {}

	void Dispatcher::dispatch(http::Client &client, conn::Monitor &monitor)
	{
		const std::string &method = client.getRequest().getMethod();

	    if (method == "TRACE")
		{
			handleTrace(client);
			return ;
	    }


	    http::Server *server = client.getServer();

	    if (!server)
	    {
			Response &response = client.getResponse();
			response.setInternalServerError();
			client.writeResponse();
			return ;
		}

		server->serve(client, monitor);
	}

    void Dispatcher::handleTrace(http::Client &client)
    {
		Response &response = client.getResponse();
		response
			.setOk()
			.setBody(client.getRequest().toString());
		client.writeResponse();
	}
}
