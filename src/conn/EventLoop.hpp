/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 16:57:28 by maurodri          #+#    #+#             */
//   Updated: 2025/10/01 23:02:50 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTLOOP_HPP
# define EVENTLOOP_HPP

#include <map>
#include "TcpServer.hpp"
#include "TcpClient.hpp"
#include "Dispatcher.hpp"
#include "Client.hpp"
#include <poll.h>
#include "Monitor.hpp"
#include <iostream>

# define MAX_EVENTS 6



namespace conn
{
	class EventLoop : public Monitor
	{
		ListEvents events;
		MapServer servers;
		MapClient clients;
		MapOperations operations;
		SetRemoveFd removeFds;
		http::Dispatcher dispatcher;

		void connectServerToClient(http::Server *server);
		void handleClientRequest(
			http::Client *client, ListEvents::iterator &eventIt);
		void handleClientWriteResponse(
			http::Client *client, ListEvents::iterator &eventIt);
		void handleFileReads(
			http::Client *client,  ListEvents::iterator &eventIt);
		void handleFileWrite(
			http::Client &client,  ListEvents::iterator &eventIt);

		void handleFdEvent(ListEvents::iterator &monitoredIt);
	    void unsubscribeFd(int fd);
	public:

		EventLoop();
		EventLoop(const EventLoop &other);
		EventLoop &operator=(const EventLoop &other);
		virtual ~EventLoop();

		bool loop(void);

		bool subscribeHttpServer(http::Server *server);
		bool subscribeHttpClient(int fd, http::Server *server);
		void unsubscribeHttpClient(ListEvents::iterator &eventIt);
		void subscribeFileRead(int fileFd, int clientFd);
		void subscribeFileWrite(int fileFd, int clientFd, std::string content);
		void shutdown(void);
	};

}

#endif
