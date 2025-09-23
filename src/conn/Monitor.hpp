/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Monitor.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 12:24:03 by maurodri          #+#    #+#             */
//   Updated: 2025/09/23 19:04:42 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#ifndef MONITOR_HPP
# define MONITOR_HPP

# include <vector>
# include <map>
# include "Client.hpp"
# include "Server.hpp"
# include <poll.h>
# include <set>

namespace conn {

	typedef std::map<int, http::Server*> MapServer;
	typedef std::map<int, http::Client*> MapClient;
	typedef std::map<int, http::Client*> MapFileReads;
	typedef std::map<int, http::Client*> MapFileWrites;
	typedef std::vector<struct pollfd> ListEvents;
	typedef std::set<int> SetRemoveFd;
	class Monitor
	{

	public:
		virtual ~Monitor() {};

		virtual bool subscribeHttpServer(http::Server *server) = 0;
		virtual bool subscribeHttpClient(int fd, http::Server *server) = 0;
		virtual void unsubscribeHttpClient(ListEvents::iterator &eventIt) = 0;
		virtual void subscribeFileRead(int fileFd, int clientFd) = 0;
		virtual void subscribeFileWrite(int fileFd, int clientFd, std::string content) = 0;
		virtual void shutdown(void) = 0;
	};

}
#endif
