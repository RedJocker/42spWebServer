// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Client.hpp                                         :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/27 17:34:03 by maurodri          #+#    #+#             //
//   Updated: 2025/08/28 20:20:17 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "TcpClient.hpp"
# include "Request.hpp"

namespace http
{

	class Client : public conn::TcpClient
	{

		Request request;
		// Response response

		Client();

	public:


		Client(const Client &other);
		Client(int clientFd);
		virtual Client &operator=(const Client &other);
		virtual Client &operator=(const conn::TcpClient &other);
		virtual ~Client();
		Request readHttpRequest();
		std::string responseAsString() const;
		std::string responseBadRequest() const;
		void clear(void);
	};
}

#endif
