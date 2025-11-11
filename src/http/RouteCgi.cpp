// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteCgi.cpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 22:34:26 by maurodri          #+#    #+#             //
//   Updated: 2025/11/10 01:20:29 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "RouteCgi.hpp"
#include "Monitor.hpp"
#include "RequestPath.hpp"
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>

namespace http {

	RouteCgi::RouteCgi() : Route()
	{
		//TODO
	}

	RouteCgi::RouteCgi(
		const std::string &pathSpecification,
		const std::string &docroot,
		const std::vector<std::string> &methodsAllowed)
		: Route(pathSpecification, docroot, methodsAllowed)
	{
	}

	RouteCgi::RouteCgi(const RouteCgi &other) : Route(other)
	{
		*this = other;
	}

	RouteCgi &RouteCgi::operator=(const RouteCgi &other)
	{
		if (this == &other)
			return *this;
		Route::operator=(other);
		return *this;
	}

	RouteCgi::~RouteCgi()
	{
		//TODO
	}

	void RouteCgi::serve(http::Client &client, conn::Monitor &monitor)
	{
		std::cout << "RouteCgi::serve: " <<  client.getFd() << std::endl;

		RequestPath &reqPath = client.getRequest().getPath();

		if (!reqPath.isFile())
		{ // if file does not exist or is directory
			client.getResponse().setNotFound();
			client.setMessageToSend(client.getResponse().toString());
			return;
		}

		int sockets[2];
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
			std::cerr << "failed to open socket: " << strerror(errno) << std::endl;
			this->onServerError(client);
			return;
		}

		pid_t pid = fork();
		if (pid < 0) {
			std::cerr << "fork failed: " << strerror(errno) << std::endl;
			close (sockets[0]);
			close (sockets[1]);
			this->onServerError(client);
			return;
		}

		if (pid == 0) {
			std::cout << "child" << std::endl;

			//prevent dangling pointer from getFilePath.c_str() after shutdown
			std::string filePathCopy = reqPath.getFilePath();
			// PHP-CGI

			const char *args[3];
			args[0] = "/usr/bin/php-cgi";
			args[1] = filePathCopy.c_str();
			args[2] = 0;

			close(sockets[1]);
			dup2(sockets[0], 0); // stdin
			dup2(sockets[0], 1); // stdout
			close(sockets[0]);
			// all child outputs to stdout now go to socket
			// and will be interpreted as cgi response

			char **envp = client.getRequest().envp();
			if (!envp)
			{
				std::cout << "Status: 500 Internal Server Error" << std::endl;
				::exit(11);
			}

			// shutdown EventLoop, careful with dangling pointers and closed fds
			monitor.shutdown();
			// nothing coming from EventLoop is valid anymore

			execve(args[0], const_cast<char **>(args), envp);

			// If execve fails, exit child
			std::cerr << "Failed to exec php-cgi: "
					  << strerror(errno) << std::endl;

			std::cerr << "Retrying in a different location " <<std::endl;
			args[0] = "/opt/homebrew/bin/php-cgi";
			execve(args[0], const_cast<char **>(args), envp);

			// Both exec fail
			std::cerr << "Failed to exec on retry: "
					  << strerror(errno) << std::endl;

			for (size_t i = 0; envp[i] != 0; ++i) {
				delete[] envp[i];
			}
			delete[] envp;

			std::cout << "Status: 500 Internal Server Error" << std::endl;

			::exit(11);
		}
		// parent
		close(sockets[0]);
		std::cout << "parent" << std::endl;
		int cgiFd = sockets[1];
		monitor.subscribeCgi(cgiFd, pid, client.getFd());
	}

	void RouteCgi::respond(http::Client &client,  const Operation &operation) const
	{
		const std::string &operationContent = operation.content;
		// Parse headers and body from operationContent
		size_t separatorIndex = operationContent.find("\r\n\r\n");
		if (separatorIndex == std::string::npos)
		{
			// TODO cgi response should always have headers and maybe contain null body
			// rewrite to interpret missing separator as headers only;
			// https://www.rfc-editor.org/rfc/rfc3875#section-6

			client.getResponse().setOk().setBody(operationContent);
			client.setMessageToSend(client.getResponse().toString());
		}
		else
		{
			std::string cgiHeadersStr = operationContent
				.substr(0, separatorIndex);
			http::Headers &cgiHeaders = client.getResponse().headers();

			size_t index = 0;
			while (1)
			{
				size_t index_next = cgiHeadersStr.find("\r\n", index);
				std::string headerLine = (index_next == std::string::npos)
					? cgiHeadersStr.substr(index)
					: cgiHeadersStr.substr(index, index_next - index);

				if (!cgiHeaders.parseLine(headerLine))
				{
					client.getResponse().setInternalServerError();
					client.setMessageToSend(client.getResponse().toString());
				}
				if (index_next == std::string::npos)
					break;
				index = index_next + 2;
			}

			Response &response = client.getResponse();
			std::string status = response.getHeader("Status");
			bool statusSet = false;
			if (status != "")
			{
				statusSet = response.setStatusCodeStr(status);
				if (statusSet && response.getStatusCode() >= 500)
				{
					response.addHeader("Connection", "close");
				}
			}
			if (!statusSet)
				response.setOk();
			else
				response.headers().eraseHeader("Status");
			response
				.setBody(operationContent.substr(separatorIndex + 4));
			client.setMessageToSend(client.getResponse().toString());
		}
	}
}
