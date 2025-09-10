// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   EventLoop.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/26 16:57:28 by maurodri          #+#    #+#             //
//   Updated: 2025/09/09 21:39:06 by maurodri         ###   ########.fr       //
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

# define MAX_EVENTS 6

namespace conn
{
	typedef std::map<int, TcpServer*>::iterator MapServerIterator;
	typedef std::map<int, http::Client*>::iterator MapClientIterator;
	typedef std::vector<struct pollfd> EventList;

	class EventLoop
	{
		std::map<int, TcpServer*> servers;
		std::map<int, http::Client*> clients;
		EventList events;
		http::Dispatcher dispatcher;

		void connectServerToClient(TcpServer *server);
		void handleClientRequest(
			http::Client *client, EventList::iterator &eventIt);
		void handleClientWriteResponse(
			http::Client *client, EventList::iterator &eventIt);
	    void unsubscribeFd(EventList::iterator &eventIt);
	public:

		EventLoop();
		EventLoop(const EventLoop &other);
		virtual EventLoop &operator=(const EventLoop &other);
		virtual ~EventLoop();

		bool subscribeTcpServer(TcpServer *tcpServer);
		bool subscribeHttpClient(int fd);
		bool loop(void);

		void unsubscribeHttpClient(EventList::iterator &eventIt);

	};

}

#endif
