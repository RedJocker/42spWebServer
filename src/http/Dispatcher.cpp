/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:41:30 by maurodri          #+#    #+#             */
//   Updated: 2025/09/17 03:28:08 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Dispatcher.hpp"
#include "Monitor.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <vector>
#include <cerrno>
#include "devUtil.hpp"
#include <sstream>
#include <string>
#include <cstdio>

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


	bool Dispatcher::isCgiRequest(http::Client &client, conn::Monitor &monitor)
	{
		// TODO delegate/move_code to http::Server
		(void) monitor;
		const std::string &path = client.getRequest().getPath();
		std::string docroot = "./www";
		std::string filePath = docroot + path;

		// TODO implement logic to check if it is cgi request based on server route config
		return filePath == "./www/todo.cgi";
	}

	void Dispatcher::handleCgiRequest(http::Client &client, conn::Monitor &monitor)
	{
		std::cout << "handleCgiRequest: " <<  client.getFd() << std::endl;
		// TODO child read stdin body of client request
		// TODO subscribe ipc fd to eventLoop through monitor
		// TODO write request body to cgi stdin
		// TODO send env to cgi with Request Meta-Variables (REQUEST_METHOD, CONTENT_LENGTH, ...)

		int sockets[2];

		if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0)
		{
			std::cerr << "errno " << errno << ": "
					  << strerror(errno) << std::endl;
			TODO("socketpair error");
		}
		switch(fork())
		{
		case 0:
		{//child;
			std::cout << "child" << std::endl;
			const std::string &path = client.getRequest().getPath();
			monitor.shutdown();
			close(sockets[1]);
			dup2(sockets[0], 0);
			dup2(sockets[0], 1);

			std::string docroot = "./www";
			std::string filePath = docroot + path;

			const char *args[3];
			args[0] = "/usr/bin/php-cgi";
			args[1] = filePath.c_str();
			args[2] = 0;

			const char *envp[1];
			envp[0] = 0;
			execve(args[0], const_cast<char **>(args), const_cast<char**>(envp));
			// execve
			//error exit
			std::cout << "error child" << strerror(errno) << std::endl;
			close(sockets[0]);
			::exit(11);
		}
		default:
		{//parent
			break;
		}}

		std::cout << "parent" << std::endl;
		close(sockets[0]);
		BufferedReader reader(sockets[1]);
		shutdown(sockets[1], SHUT_WR);
		std::pair<ReadState, char *> readResult;
		while(readResult.first == BufferedReader::READING)
		{
			std::cout << "parent reading" << std::endl;
			readResult = reader.readAll();
		}
		std::cout << "parent done reading" << std::endl;
		if (readResult.first != BufferedReader::NO_CONTENT)
		{
			std::cout << "error exit" << std::endl;
			// TODO error on cgi reading
			client.getResponse().setInternalServerError();
			client.setMessageToSend(client.getResponse().toString());
			return  ;
		}

		std::string cgiResponseString = std::string(readResult.second);
		std::cout << cgiResponseString << std::endl;

		size_t separatorIndex = cgiResponseString.find("\r\n\r\n");
		if (separatorIndex == std::string::npos)
		{
			std::cout << cgiResponseString << std::endl;
			TODO("cgi response had no header body separation");
			// either respond 500 or consider all cgi content as response body
		}

		std::string cgiHeadersStr =  cgiResponseString.substr(0, separatorIndex);

		Headers &cgiHeaders = client.getResponse().headers();

		size_t index = 0;
		while(1)
		{
			size_t index_next = cgiHeadersStr.find("\r\n", index);
			std::string headerLine;
			if (index_next == std::string::npos)
			{
				 headerLine = cgiHeadersStr.substr(index);
			} else
			{
				headerLine = cgiHeadersStr
					.substr(index, index_next - index);
			}
			std::cout << index_next << std::endl;
			std::cout << headerLine <<std::endl;
		    if (!cgiHeaders.parseLine(headerLine))
		    {
				client.getResponse().setInternalServerError();
				client.setMessageToSend(client.getResponse()
										.toString());
			}
			if (index_next == std::string::npos)
				break;
			index = index_next + 2;
		}
		client.getResponse().setOk().setBody(cgiResponseString.substr(separatorIndex + 4));
		client.setMessageToSend(client.getResponse().toString());
		std::string childMessage = std::string(readResult.second);
		std::cout << "childMessage: " << childMessage;
		delete[] readResult.second;
		close(sockets[1]);
	}

	void Dispatcher::dispatch(http::Client &client, conn::Monitor &monitor)
	{
		const std::string &method = client.getRequest().getMethod();
		Response &response = client.getResponse();

		if (method == "TRACE") {
			handleTrace(client, response);
			return ;
		}

		if (isCgiRequest(client, monitor))
		{
			handleCgiRequest(client, monitor);
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
