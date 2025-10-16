/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 13:05:25 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/16 15:57:05 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Monitor.hpp"
#include "pathUtils.hpp"
#include "devUtil.hpp"
#include "RequestPath.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <cerrno>
#include <cstdlib>
#include <cstring>

namespace http
{
	const std::string Server::DEFAULT_DOCROOT = "./www";
	Server::Server(const std::string &hostname,
				   const std::string &docroot,
				   unsigned short port)
		: conn::TcpServer(port), hostname(hostname), docroot(docroot)
	{
		this->docroot = docroot;
		while (!this->docroot.empty()
			   && this->docroot[this->docroot.size() - 1] == '/')
		{
			// keeping docroot without ending /
			// must ensure path has leading / on RequestPath
			this->docroot.erase(this->docroot.end() - 1);
		}
		this->docroot = this->docroot.empty() ?
			DEFAULT_DOCROOT : this->docroot;
	}

	Server::Server(const Server &other) : conn::TcpServer(other.port)
	{
		*this = other;
	}

	Server &Server::operator=(const Server &other)
	{
		if (this != &other)
		{
			this->hostname = other.hostname;
			this->docroot = other.docroot;
			this->port = other.port;
		}
		return *this;
	}

	Server::~Server(void) {}

	const std::string &Server::getHostname() const
	{
		return this->hostname;
	}

	const std::string &Server::getDocroot() const
	{
		return this->docroot;
	}

	void Server::addCgiRoute(const std::string &route)
	{
		for (std::vector<std::string>::const_iterator it = cgiRoutes.begin();
			 it != cgiRoutes.end();
			 ++it)
		{
			if (*it == route)
				return;
		}
		cgiRoutes.push_back(route);
	}

	const std::vector<std::string> &Server::getCgiRoutes() const
	{
		return cgiRoutes;
	}


	bool Server::isCgiRequest(http::Client &client, conn::Monitor &monitor)
	{
		(void) monitor;
		RequestPath &reqPath = client.getRequest().getPath();

		// Check if this path matches any of the server's configured CGI routes
		const std::vector<std::string> &cgiRoutes = this->getCgiRoutes();
		for (std::vector<std::string>::const_iterator it = cgiRoutes.begin();
			 it != cgiRoutes.end();
			 ++it)
		{
			// TODO make possible to match on fileExtension ex "/42/*.cgi"
			std::string expectedPath = this->getDocroot() + *it;

			if (reqPath.getFilePath() == expectedPath)
				return true;
		}

		return false;
	}

	void Server::handleCgiRequest(http::Client &client, conn::Monitor &monitor)
	{
		std::cout << "handleCgiRequest: " <<  client.getFd() << std::endl;

		RequestPath &reqPath = client.getRequest().getPath();

		if (!reqPath.isFile())
		{
			client.getResponse().setNotFound();
			client.setMessageToSend(client.getResponse().toString());
			return;
		}

		int sockets[2];
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0)
		{
			std::cerr << "errno " << errno << ": "
					  << strerror(errno) << std::endl;
			throw std::runtime_error("socketpair error");
		}

		switch (fork())
		{
		case 0:
		{ //child
			std::cout << "child" << std::endl;

			//Init CGI env
			char **envp = client.getRequest().envp();
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
			monitor.shutdown(); // shutdown EventLoop

			execve(args[0], const_cast<char **>(args), envp);

			// TODO handle script errors like infinity loop, runtime error, etc
			// we will need to create some request timeout system


			// If execve fails, exit child
			std::cerr << "Failed to exec php-cgi: "
					  << strerror(errno) << std::endl;

			std::cerr << "Retrying in a different location " <<std::endl;
			args[0] = "/opt/homebrew/bin/php-cgi";
			execve(args[0], const_cast<char **>(args), envp);

			std::cerr << "Failed to exec on retry: "
					  << strerror(errno) << std::endl;

			for (size_t i = 0; envp[i] != 0; ++i) {
				delete envp[i];
			}
			delete[] envp;

			::exit(11);
		}
		default: { break; }
		}
		//parent
		std::cout << "parent" << std::endl;
		close(sockets[0]); // close child fd

		int cgiFd = sockets[1];
		monitor.subscribeCgi(cgiFd, client.getFd());
	}

	void Server::onFileRead(http::Client &client, const std::string &fileContent)
	{
		RequestPath &reqPath = client.getRequest().getPath();


		std::string mimeType = utils::guessMimeType(reqPath.getExtension());
		client.getResponse()
			.addHeader("Content-Type", mimeType)
			.setOk()
			.setBody(fileContent);
		client.setMessageToSend(client.getResponse().toString());
	}

	void Server::onFileWritten(http::Client &client)
	{
		client.getResponse()
			.setCreated();
		client.setMessageToSend(client.getResponse().toString());
	}

	void Server::onServerError(http::Client &client)
	{
		client.getResponse()
			.setInternalServerError();
		client.setMessageToSend(client.getResponse().toString());
	}

	void Server::onCgiResponse(
		http::Client &client, const std::string cgiResponse)
	{
		// Parse headers and body from cgiResponse
		size_t separatorIndex = cgiResponse.find("\r\n\r\n");
		if (separatorIndex == std::string::npos)
		{
			client.getResponse().setOk().setBody(cgiResponse);
			client.setMessageToSend(client.getResponse().toString());
		}
		else
		{
			std::string cgiHeadersStr = cgiResponse
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
			client.getResponse()
				.setOk()
				.setBody(cgiResponse.substr(separatorIndex + 4));
			client.setMessageToSend(client.getResponse().toString());
		}
	}
	void Server::handleGetFile(http::Client &client, conn::Monitor &monitor)
	{
		std::cout << "Server::handleGetFile " << std::endl;


		RequestPath &reqPath = client.getRequest().getPath();

		int fd = open(reqPath.getFilePath().c_str(), O_RDONLY);
		if (fd < 0) {
			// TODO
			// we need to check reason failed to open
			// and give a response based on the reason
			client.getResponse().setNotFound();
			client.setMessageToSend(client.getResponse().toString());
			return;
		}
		std::cout << "clientFd = " << client.getFd() << std::endl;
		monitor.subscribeFileRead(fd, client.getFd());
	}

	void Server::handleGetDirectory(
		http::Client &client, conn::Monitor &monitor)
	{
		(void) monitor;

		const RequestPath &reqPath = client.getRequest().getPath();
		Response &response = client.getResponse();

		const std::string &dirPath = reqPath.getFilePath();

		DIR *dir = opendir(dirPath.c_str());
		if (!dir)
		{
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
		for (std::vector<std::string>::const_iterator it = entries.begin();
			 it != entries.end();
			 ++it)
		{
			html << "<li><a href=\"./" << *it << "\">" << *it << "</a></li>";
		}
		html << "</ul></body></html>";

		response.setOk()
			.addHeader("Content-Type", "text/html")
			.setBody(html.str());
		client.setMessageToSend(response.toString());
	}

	void Server::handlePost(http::Client &client, conn::Monitor &monitor)
	{

		const RequestPath &reqPath = client.getRequest().getPath();

		const std::string &filePath = reqPath.getFilePath();
		Response &response = client.getResponse();

		int fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
		{
			// TODO
			// we need to check reason failed to open
			// and give a response based on the reason
			std::cerr << "Failed to open file: " << filePath << std::endl;
			response.setInternalServerError();
			client.setMessageToSend(response.toString());
			return;
		}

		monitor.subscribeFileWrite(
			fd, client.getFd(), client.getRequest().getBody());
	}

	void Server::handleDelete(http::Client &client, conn::Monitor &monitor)
	{
		// TODO
		// what to do if file is directory? delete folder ? error ?

		(void) monitor;

		const std::string &filePath =
			client.getRequest().getPath().getFilePath();
		Response &response = client.getResponse();

		int result = unlink(filePath.c_str());
		if (result == 0)
		{
			response.clear();
			response.setStatusCode(204);
			response.setStatusInfo("No Content");
		} else {
			// TODO
			// we need to check reason failed to unlink
			// and give a response based on the reason
			// it may be not found, permission issue, maybe something else
			response.setNotFound();
		}

		client.setMessageToSend(response.toString());
	}


	void Server::serve(Client &client, conn::Monitor &monitor)
	{
		RequestPath &reqPath = client.getRequest().getPath();

		// Redirect if not '/' for directory listing
		if (reqPath.isDirectory() && reqPath.needsTrailingSlashRedirect()) {
			Response &response = client.getResponse();
			std::string location = reqPath.getPath() + "/";
			response.clear();
			response.setStatusCode(308);
			response.setStatusInfo("Permanent Redirect");
			response.addHeader("Location", location);
			response.addHeader("Content-Length", "0");
			client.setMessageToSend(response.toString());
			return;
		}

		if (isCgiRequest(client, monitor))
		{
			handleCgiRequest(client, monitor);
			return ;
		}

		const std::string &method = client.getRequest().getMethod();
		Response &response = client.getResponse();

		if (method == "GET" && (reqPath.isFile() || reqPath.isDirectory()))
		{
			if (reqPath.isDirectory())
				handleGetDirectory(client, monitor);
			else
				handleGetFile(client, monitor);

			return;
		}

		if (method == "POST")
		{
			handlePost(client, monitor);
			return ;
		}

		if (method == "DELETE" && (reqPath.isFile() || reqPath.isDirectory()))
		{
			handleDelete(client, monitor);
			return ;
		}

		response.setNotFound();
		client.setMessageToSend(response.toString());
	}
}
