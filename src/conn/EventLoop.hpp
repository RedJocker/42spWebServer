// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   EventLoop.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/26 16:57:28 by maurodri          #+#    #+#             //
//   Updated: 2025/08/27 18:11:59 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef EVENTLOOP_HPP
# define EVENTLOOP_HPP

#include <map>
#include "TcpServer.hpp"
#include "TcpClient.hpp"
#include "Client.hpp"
#include <sys/epoll.h>

# define MAX_EVENTS 6

namespace conn
{

	class EventLoop
	{
		int epollFd;
		std::map<int, TcpServer*> servers;
		std::map<int, http::Client*> clients;

	public:

		EventLoop();
		EventLoop(const EventLoop &other);
		virtual EventLoop &operator=(const EventLoop &other);
		virtual ~EventLoop();

		bool subscribeTcpServer(TcpServer *tcpServer);
		bool subscribeHttpClient(int fd);
		bool loop(void);
		bool isOk() const;
		bool unsubscribeHttpClient(http::Client *client , struct epoll_event *clientEvent);

	};

	typedef std::map<int, TcpServer*>::iterator MapServerIterator;
	typedef std::map<int, http::Client*>::iterator MapClientIterator;
}

#endif
