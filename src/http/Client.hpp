/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:34:03 by maurodri          #+#    #+#             */
//   Updated: 2025/09/09 17:49:38 by maurodri         ###   ########.fr       //
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
		Client &operator=(const Client &other);
		virtual ~Client();
		Request &readHttpRequest();
		Response &getResponse();
		Request &getRequest();

		void clear(void);
	};
}

#endif
