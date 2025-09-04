/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 17:41:12 by maurodri          #+#    #+#             */
/*   Updated: 2025/09/04 12:28:18 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DISPATCHER_HPP
# define DISPATCHER_HPP

#include "TcpServer.hpp"
#include "Client.hpp"

namespace http
{

	typedef std::map<int, conn::TcpServer*> MapServer;

	class Dispatcher {

		Response &handleTrace(http::Client &client, Response &response);
		Response &handleGetFile(http::Client &client, Response &response);

	public:

		Dispatcher();
		Dispatcher(const Dispatcher &other);
		virtual Dispatcher &operator=(const Dispatcher &other);
		virtual ~Dispatcher();

		http::Response &dispatch(http::Client &client,  http::MapServer &servers);
	};
}

#endif
