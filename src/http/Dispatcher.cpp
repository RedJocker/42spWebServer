/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:41:30 by maurodri          #+#    #+#             */
/*   Updated: 2025/09/25 18:37:52 by vcarrara         ###   ########.fr       */
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
	
	http::Server &Dispatcher::resolveServer(http::Client &client) {
		http::Server *srv = client.getServer();
		if (!srv)
			throw std::runtime_error("Client has no associated server");
		return *srv;
	}

	bool Dispatcher::isCgiRequest(http::Client &client, conn::Monitor &monitor)
	{
		(void)monitor;
		
		// Resolve server for this client docroot CGI route
		http::Server &server = resolveServer(client);
		const std::string &path = client.getRequest().getPath();
		
		// Normalize requested path
		std::string filePath;
		if (!utils::normalizeUrlPath(server.getDocroot(), path, filePath))
			return false;
			
		// Check if this path matches any of the server's configured CGI routes
		const std::vector<std::string> &cgiRoutes = server.getCgiRoutes();
		for (std::vector<std::string>::const_iterator it = cgiRoutes.begin(); it != cgiRoutes.end(); ++it) {
			std::string expectedPath = server.getDocroot() + "/" + *it;
			if (filePath == expectedPath)
				return true; // Path matches a configured CGI route
		}
		
		return false; // No match found
	}

	void Dispatcher::handleCgiRequest(http::Client &client, conn::Monitor &monitor)
	{
		std::cout << "handleCgiRequest: " <<  client.getFd() << std::endl;
		 
		int sockets[2];
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0)
		{
			std::cerr << "errno " << errno << ": "
					  << strerror(errno) << std::endl;
			throw std::runtime_error("socketpair error");
		}
		
		switch(fork()) {
			case 0: { //child
				std::cout << "child" << std::endl;
				const std::string &path = client.getRequest().getPath();
				
				// Init CGI env
				std::vector<char *> envp;
				client.getRequest().envpInit(envp);

				monitor.shutdown(); // shutdown EventLoop
				close(sockets[1]);
				dup2(sockets[0], 0); // stdin
				dup2(sockets[0], 1); // stdout

				// Dynamic server docroot
				std::string docroot = resolveServer(client).getDocroot();
				std::string filePath;
				if (!utils::normalizeUrlPath(docroot, path, filePath)) {
					client.getResponse().setNotFound();
					client.setMessageToSend(client.getResponse().toString());
					close(sockets[0]);
					return;
				}

				// PHP-CGI
				const char *args[3];
				args[0] = "/usr/bin/php-cgi";
				args[1] = filePath.c_str();
				args[2] = 0;
				
				execve(args[0],
						const_cast<char **>(args),
						reinterpret_cast<char **>(envp.data()));
				
				// If execve fails, exit child
				std::cerr << "Failed to exec php-cgi: " << strerror(errno) << std::endl;
				close(sockets[0]);
				::exit(11);
			}
			
			default: { //parent
				std::cout << "parent" << std::endl;
				close(sockets[0]); // close child

				// Write request body to CGI
				std::cout << "parent writing to cgi: " << std::endl;
				BufferedWriter writer(sockets[1]);
				writer.setMessage("hello=there&abc=def");
				std::pair<WriteState, char *> flushResult(
					BufferedWriter::WRITING, 0);
				std::cout << writer.getState() << std::endl;
				while (flushResult.first == BufferedWriter::WRITING)
					flushResult = writer.flushMessage();
				shutdown(sockets[1],SHUT_WR);
				
				// Read CGI response
				std::cout << "parent done writing to cgi:" << std::endl;
				BufferedReader reader(sockets[1]);
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
					delete[] readResult.second;
					close(sockets[1]);
					return ;
				}

				std::string cgiResponseString(readResult.second);
				delete[] readResult.second;
				close(sockets[1]);

				std::cout << "CGI Response: "<< cgiResponseString << std::endl;

				// Parse headers and body
				size_t separatorIndex = cgiResponseString.find("\r\n\r\n");
				if (separatorIndex == std::string::npos) {
					client.getResponse().setOk().setBody(cgiResponseString);
					client.setMessageToSend(client.getResponse().toString());
				} else {
					std::string cgiHeadersStr = cgiResponseString.substr(0, separatorIndex);
					Headers &cgiHeaders = client.getResponse().headers();

					size_t index = 0;
					while(1) {
						size_t index_next = cgiHeadersStr.find("\r\n", index);
						std::string headerLine = (index_next == std::string::npos)
							? cgiHeadersStr.substr(index)
							: cgiHeadersStr.substr(index, index_next - index);
							
 						if (!cgiHeaders.parseLine(headerLine)) {
							client.getResponse().setInternalServerError();
							client.setMessageToSend(client.getResponse().toString());
						}
						
						if (index_next == std::string::npos)
							break;
						index = index_next + 2;
					}
					client.getResponse()
						.setOk()
						.setBody(cgiResponseString.substr(separatorIndex + 4));
					client.setMessageToSend(client.getResponse().toString());
				}
			}
		}
	}

	void Dispatcher::dispatch(http::Client &client, conn::Monitor &monitor)
	{
		const std::string &method = client.getRequest().getMethod();
		Response &response = client.getResponse();
		http::Server *server = client.getServer();

		if (!server) {
			response.setInternalServerError();
			client.setMessageToSend(response.toString());
			return ;
		}

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
			std::string docroot = server->getDocroot();
			std::string filePath;
			if (!utils::normalizeUrlPath(docroot, path, filePath)) {
				response.setNotFound();
				client.setMessageToSend(response.toString());
				return ;
			}

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
		std::string docroot = resolveServer(client).getDocroot();
		std::string filePath;
		if (!utils::normalizeUrlPath(docroot, path, filePath)) {
			response.setNotFound();
			client.setMessageToSend(response.toString());
			return ;
		}

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
		std::string docroot = resolveServer(client).getDocroot();
		std::string dirPath;
		if (!utils::normalizeUrlPath(docroot, path, dirPath)) {
			response.setNotFound();
			client.setMessageToSend(response.toString());
			return ;
		}

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
			entries.push_back(name);
		}
		closedir(dir);

		std::stringstream html;
		html << "<html><body>"
			 << "<h1> Index of " << dirPath.substr(docroot.size())
			 << "</h1>"
			 << "<hr>"
			 << "<ul>";
		for (std::vector<std::string>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
			html << "<li><a href=\"" << *it << "\">" << *it << "</a></li>";
		}
		html << "</ul></body></html>";

		response.setOk()
			.setBody(html.str());
		client.setMessageToSend(response.toString());
	}

	void Dispatcher::handlePost(http::Client &client, Response &response, conn::Monitor &monitor) {
		const std::string &path = client.getRequest().getPath();
		std::string docroot = resolveServer(client).getDocroot();
		std::string filePath;
		if (!utils::normalizeUrlPath(docroot, path, filePath)) {
			response.setNotFound();
			client.setMessageToSend(response.toString());
			return ;
		}

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
		std::string docroot = resolveServer(client).getDocroot();
		std::string filePath;
		if (!utils::normalizeUrlPath(docroot, path, filePath)) {
			response.setNotFound();
			client.setMessageToSend(response.toString());
			return ;
		}

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
