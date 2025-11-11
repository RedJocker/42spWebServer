/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 16:57:28 by maurodri          #+#    #+#             */
/*   Updated: 2025/11/05 20:23:03 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTLOOP_HPP
# define EVENTLOOP_HPP

#include "Dispatcher.hpp"
#include "Client.hpp"
#include "Monitor.hpp"

#include <poll.h>
#include <ctime>

namespace conn
{
	class EventLoop : public Monitor
	{

		ListEvents events;
		MapServer servers;
		MapClient clients;
		MapOperations operations;
		SetRemoveFd removeFds;
		ListEvents subscribeFds;

		http::Dispatcher dispatcher;

		void connectServerToClient(http::Server *server);
		void handleClientRequest(
			http::Client *client, ListEvents::iterator &eventIt);
		void handleClientWriteResponse(
			http::Client *client, ListEvents::iterator &eventIt);
		void handleFileReads(const Operation &op, http::Client &client);
		void handleFileWrite(const Operation &op, http::Client &client);
		void handleCgiWrite(const Operation &op, http::Client &client);
		void handleCgiRead(const Operation &op, http::Client &client);

		void handleFdEvent(ListEvents::iterator &monitoredIt);
		void unsubscribeFd(int fd);
		void unsubscribeOperation(int operationFd);
		time_t markExpiredOperations(void);
	public:
		static bool shouldExit;
		static time_t timeoutLimit;

		EventLoop();
		EventLoop(const EventLoop &other);
		EventLoop &operator=(const EventLoop &other);
		virtual ~EventLoop();

		bool loop(void);

		// Monitor required overrides
		bool subscribeHttpServer(http::Server *server);
		bool subscribeHttpClient(int fd, http::Server *server);
		void unsubscribeHttpClient(ListEvents::iterator &eventIt);
		void subscribeFileRead(int fileFd, int clientFd);
		void subscribeFileWrite(int fileFd, int clientFd, std::string content);
		void subscribeCgi(int fileFd, pid_t cgiFd, int clientFd);
		void shutdown(void);
	};

}

#endif
