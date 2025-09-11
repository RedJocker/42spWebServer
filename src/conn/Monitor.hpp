// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Monitor.hpp                                        :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/09/10 12:24:03 by maurodri          #+#    #+#             //
//   Updated: 2025/09/10 23:31:41 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef MONITOR_HPP
# define MONITOR_HPP

# include <vector>
# include <map>
# include "Client.hpp"
# include "TcpServer.hpp"
# include <poll.h>
# include <set>

namespace conn {

	typedef std::map<int, TcpServer*> MapServer;
	typedef std::map<int, http::Client*> MapClient;
	typedef std::map<int, http::Client*> MapFileReads;
	typedef std::vector<struct pollfd> ListEvents;
	typedef std::set<int> SetRemoveFd;
	class Monitor
	{

	public:
		virtual ~Monitor() {};

		virtual bool subscribeTcpServer(TcpServer *tcpServer) = 0;
		virtual bool subscribeHttpClient(int fd) = 0;
		virtual void unsubscribeHttpClient(ListEvents::iterator &eventIt) = 0;
		virtual void subscribeFileRead(int fileFd, int clientFd) = 0;
	};

}
#endif
