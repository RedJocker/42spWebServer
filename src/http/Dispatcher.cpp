/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:41:30 by maurodri          #+#    #+#             */
//   Updated: 2025/09/04 17:53:50 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Dispatcher.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>

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
		Response &response = client.getResponse();

		if (method == "TRACE")
			return handleTrace(client, response);

		if (method == "GET")
			return handleGetFile(client, response);
		return response.setNotFound();
	}

	Response &Dispatcher::handleTrace(http::Client &client, Response &response) {
		return response
			.setOk()
			.setBody(client.getRequest().toString());
	}

	Response &Dispatcher::handleGetFile(http::Client &client, Response &response) {
		const std::string &path = client.getRequest().getPath();
		std::string docroot = "./www";
		std::string filePath = docroot + path;

		int fd = open(filePath.c_str(), O_RDONLY);
		if (fd < 0)
			return response.setNotFound();

		// TODO: subscribe fd on EventLoop, maybe reuse reader from client
		BufferedReader reader(fd);
		std::pair<BufferedReader::ReadState, char*> readResult;
		readResult.first = BufferedReader::READING;
		while(readResult.first == BufferedReader::READING)
		{
			readResult = reader.readAll();
		}

		std::string body = "";
		if (readResult.first == BufferedReader::NO_CONTENT)
		{
			body = std::string(readResult.second);
			delete[] readResult.second;
		}

		return response
			.setOk()
			.addHeader("Content-Type", "text/html")
			.setBody(body);
	}
}
