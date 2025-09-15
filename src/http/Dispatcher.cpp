/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:41:30 by maurodri          #+#    #+#             */
/*   Updated: 2025/09/15 11:10:55 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dispatcher.hpp"
#include "Monitor.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

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
		Response &response = client.getResponse();

		if (method == "TRACE") {
			handleTrace(client, response);
			return ;
		}

		if (method == "GET") {
			handleGetFile(client, response, monitor);
			return ;
		}

		if (method == "POST") {
			// call this to subscribe writing to file
			//monitor.subscribeFileWrite(int fileFd, int clientFd, std::string content)

			const std::string &path = client.getRequest().getPath();
			std::string docroot = "./www";
			std::string filePath = docroot + path;

			int fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd < 0) {
				std::cerr << "Failed to open file: " << filePath << std::endl;
				response.setBadRequest();
				client.setMessageToSend(response.toString());
				return;
			}

			const std::string &body = client.getRequest().getBody();
			ssize_t bytesWritten = write(fd, body.c_str(), body.size());
			if (bytesWritten < 0 || static_cast<size_t>(bytesWritten) != body.size()) {
				std::cerr << "Failed to write entire body to file: " << filePath << std::endl;
				response.setBadRequest();
				client.setMessageToSend(response.toString());
				close(fd);
				return;
			}

			close(fd);
			response.setCreated();
			client.setMessageToSend(response.toString());
			return ;
		}

		response.setNotFound();
		client.setMessageToSend(response.toString());
	}

	void Dispatcher::handleTrace(http::Client &client, Response &response) {
		response
			.setOk()
			.setBody(client.getRequest().toString());
		client.setMessageToSend(response.toString());
	}

	void Dispatcher::handleGetFile(
		http::Client &client, Response &response, conn::Monitor &monitor) {

		std::cout << "Disp::handleGetFile " << std::endl;
		const std::string &path = client.getRequest().getPath();
		std::string docroot = "./www";
		std::string filePath = docroot + path;

		int fd = open(filePath.c_str(), O_RDONLY);
		if (fd < 0) {
			response.setNotFound();
			client.setMessageToSend(response.toString());
			return ;
		}
		std::cout << "clientFd = " << client.getFd() << std::endl;
		// TODO: subscribe fd on EventLoop, maybe reuse reader from client
		monitor.subscribeFileRead(fd, client.getFd());
	}
}
