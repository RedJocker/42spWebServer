// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   EventLoop.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/26 16:57:28 by maurodri          #+#    #+#             //
//   Updated: 2025/09/16 00:46:58 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

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
		MapServer servers;
		MapClient clients;
		MapFileReads fileReads;
		MapFileWrites fileWrites;
		ListEvents events;
		SetRemoveFd removeFds;
		http::Dispatcher dispatcher;

		void connectServerToClient(TcpServer *server);
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

		bool subscribeTcpServer(TcpServer *tcpServer);
		bool subscribeHttpClient(int fd);
		void unsubscribeHttpClient(ListEvents::iterator &eventIt);
		void subscribeFileRead(int fileFd, int clientFd);
		void subscribeFileWrite(int fileFd, int clientFd, std::string content);
	};

}

#endif
