/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:34:03 by maurodri          #+#    #+#             */
//   Updated: 2025/09/03 20:33:00 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "TcpClient.hpp"
# include "Request.hpp"
# include "Response.hpp"

namespace http
{

	class Client : public conn::TcpClient
	{

		Request request;
		Response response;

		Client();

	public:


		Client(const Client &other);
		Client(int clientFd);
		virtual Client &operator=(const Client &other);
		virtual Client &operator=(const conn::TcpClient &other);
		virtual ~Client();
		Request &readHttpRequest();
		Response &getResponse();
		Request &getRequest();

		void clear(void);
	};
}

#endif
