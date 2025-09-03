// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Dispatcher.hpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/09/03 17:41:12 by maurodri          #+#    #+#             //
//   Updated: 2025/09/03 19:19:32 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef DISPATCHER_HPP
# define DISPATCHER_HPP

#include "TcpServer.hpp"
#include "Client.hpp"

namespace http
{

	typedef std::map<int, conn::TcpServer*> MapServer;
	
	class Dispatcher
	{

	

	public:

		Dispatcher();
		Dispatcher(const Dispatcher &other);
		virtual Dispatcher &operator=(const Dispatcher &other);
		virtual ~Dispatcher();

		http::Response &dispatch(http::Client &client,  http::MapServer &servers);
	};

}

#endif
