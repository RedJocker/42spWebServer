/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:41:12 by maurodri          #+#    #+#             */
/*   Updated: 2025/09/15 11:42:21 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DISPATCHER_HPP
# define DISPATCHER_HPP

#include "Monitor.hpp"
#include "TcpServer.hpp"
#include "Client.hpp"

namespace http
{

	typedef std::map<int, conn::TcpServer*> MapServer;

	class Dispatcher {

		void handleTrace(http::Client &client, Response &response);
		void handleGetFile(
			http::Client &client, Response &response, conn::Monitor &monitor);
		void handleGetDirectory(
			http::Client &client, Response &response);
		void handlePost(http::Client &client, Response &response);
		void handleDelete(http::Client &client, Response &response);

	public:

		Dispatcher();
		Dispatcher(const Dispatcher &other);
		Dispatcher &operator=(const Dispatcher &other);
		virtual ~Dispatcher();

		void dispatch(http::Client &client,  conn::Monitor &monitor);
	};
}

#endif
