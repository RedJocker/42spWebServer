// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   EventLoop.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/26 16:57:28 by maurodri          #+#    #+#             //
//   Updated: 2025/08/26 22:31:23 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef EVENTLOOP_HPP
# define EVENTLOOP_HPP

#include <map>
#include "TcpServer.hpp"
#include "TcpClient.hpp"
#include <sys/epoll.h>

# define MAX_EVENTS 6

namespace conn
{

	class EventLoop
	{
		int epollFd;
		std::map<int, TcpServer*> servers;
		std::map<int, TcpClient*> clients;

	public:

		EventLoop();
		EventLoop(const EventLoop &other);
		virtual EventLoop &operator=(const EventLoop &other);
		virtual ~EventLoop();

		bool subscribeTcpServer(TcpServer *tcpServer);
		bool subscribeTcpClient(int fd);
		bool loop(void);
		bool isOk() const;
		bool unsubscribeTcpClient(TcpClient *client , struct epoll_event *clientEvent);

	};

	typedef std::map<int, TcpServer*>::iterator MapServerIterator;
	typedef std::map<int, TcpClient*>::iterator MapClientIterator;
}

#endif
