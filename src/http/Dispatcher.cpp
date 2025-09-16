/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:41:30 by maurodri          #+#    #+#             */
//   Updated: 2025/09/15 22:57:31 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Dispatcher.hpp"
#include "Monitor.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <vector>

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
			const std::string &path = client.getRequest().getPath();
			std::string docroot = "./www";
			std::string filePath = docroot + path;

			struct stat pathStat;
			if (stat(filePath.c_str(), &pathStat) != 0) {
				response.setNotFound();
				client.setMessageToSend(response.toString());
				return ;
			}

			if (S_ISDIR(pathStat.st_mode))
				handleGetDirectory(client, response);
			else
				handleGetFile(client, response, monitor);

			return ;
		}

		if (method == "POST") {
			handlePost(client, response, monitor);
			return ;
		}

		if (method == "DELETE") {
			handleDelete(client, response);
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

	void Dispatcher::handleGetDirectory(http::Client &client, Response &response) {
		const std::string &path = client.getRequest().getPath();
		std::string docroot = "./www";
		std::string dirPath = docroot + path;

		DIR *dir = opendir(dirPath.c_str());
		if (!dir) {
			response.setNotFound();
			client.setMessageToSend(response.toString());
			return ;
		}

		struct dirent *entry;
		std::vector<std::string> entries;
		while ((entry = readdir(dir)) != NULL) {
			std::string name(entry->d_name);
			if (name != "." && name != "..") {
				entries.push_back(name);
			}
		}
		closedir(dir);

		std::string html = "<html><body><ul>";
		for (std::vector<std::string>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
			html += "<li><a href=\"" + *it + "\">" + *it + "</a></li>";
		}
		html += "</ul></body></html>";

		response.setOk()
			.setBody(html);
		client.setMessageToSend(response.toString());
	}

	void Dispatcher::handlePost(http::Client &client, Response &response, conn::Monitor &monitor) {
		const std::string &path = client.getRequest().getPath();
		std::string docroot = "./www";
		std::string filePath = docroot + path;

		int fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0) {
			std::cerr << "Failed to open file: " << filePath << std::endl;
			response.setInternalServerError();
			client.setMessageToSend(response.toString());
			return;
		}

		monitor.subscribeFileWrite(fd, client.getFd(), client.getRequest().getBody());
	}

	void Dispatcher::handleDelete(http::Client &client, Response &response) {
		const std::string &path = client.getRequest().getPath();
		std::string docroot = "./www";
		std::string filePath = docroot + path;

		int result = unlink(filePath.c_str());
		if (result == 0) {
			response.clear();
			response.setStatusCode(204);
			response.setStatusInfo("No Content");
		} else {
			response.setNotFound();
		}

		client.setMessageToSend(response.toString());
	}
}
