/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 10:41:51 by vcarrara          #+#    #+#             */
//   Updated: 2025/08/27 19:25:49 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include "Headers.hpp"
#include "Body.hpp"
#include "TcpClient.hpp"

namespace http
{
	class Request
	{
	public:

		enum ReadState {
			READING_REQUEST_LINE,
			READING_HEADERS,
			READING_BODY,
			READ_ERROR,
			READ_BAD_REQUEST,
			READ_COMPLETE,
			READ_EOF
		};

		Request(void);
		Request(const Request &other);
		virtual Request &operator=(const Request &other);
		virtual ~Request(void);

		ReadState readFromTcpClient(conn::TcpClient &client);
		ReadState state(void) const { return _state; }
		void clear(void);

		std::string getMethod(void) const;
		std::string getPath(void) const;
		std::string getProtocol(void) const;
		std::string getHeader(const std::string &key) const;
		std::string getBody(void) const;

	private:
		std::string _method;
		std::string _path;
		std::string _protocol;

		Headers _headers;
		Body _body;

		ReadState _state;

		bool parseRequestLine(const std::string &line);
	};
}

#endif
