/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:41:30 by maurodri          #+#    #+#             */
/*   Updated: 2025/09/04 12:06:25 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dispatcher.hpp"

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

	Response &Dispatcher::dispatch(http::Client &client,  http::MapServer &servers)
	{
		(void) servers;
		const std::string &method = client.getRequest().getMethod();
		const std::string &path = client.getRequest().getPath();
		Response &response = client.getResponse();

		if (method == "TRACE") {
			return response
				.setOk()
				.setBody(client.getRequest().toString());
		}

		if (method == "GET") {
			std::string docroot = "./www";
			std::string filePath = docroot + path;

			int fd = open(filePath.c_str(), 0_RDONLY | 0_NONBLOCK);
			if (fd < 0)
				return response.setNotFound();

			struct stat st;
			if (::fstat(fd, &st) == 0) {
				std::ostringstream oss;
				oss << st.st_size;
				response.addHeader("Content-Length", oss.str());
			}

			return response
				.setOk()
				.addHeader("Content-Type", "text/html")
				.setFileBody(fd);
		}

		return response.setNotFound();
	}
}
