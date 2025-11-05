/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 10:41:51 by vcarrara          #+#    #+#             */
//   Updated: 2025/11/05 00:21:35 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include "Headers.hpp"
#include "Body.hpp"
#include "TcpClient.hpp"
#include "RequestPath.hpp"

namespace http
{
	class Request
	{
	public:

		enum ReadState {
			READING_REQUEST_LINE,
			READING_HEADERS,
			READING_BODY,
			READING_CHUNKED_BODY,
			READ_ERROR,
			READ_BAD_REQUEST,
			READ_COMPLETE,
			READ_EOF
		};

		Request(void);
		Request(const Request &other);
		Request &operator=(const Request &other);
		virtual ~Request(void);

		ReadState readHttpRequest(BufferedReader &reader);
		ReadState state(void) const { return _state; }
		void clear(void);

		const std::string &getMethod(void) const;
		const std::string &getPathRaw(void) const;
		const std::string &getProtocol(void) const;

		RequestPath &getPath(void);
		std::string getHeader(const std::string &key) const;
		std::string getBody(void) const;

		std::string toString(void) const;
		char **envp(void) const;

		bool hasMultipart(void) const;
		const std::string &getMultipartBoundary(void) const;

	private:
		std::string _method;
		std::string _pathRaw;
		std::string _protocol;

		RequestPath _path;
		Headers _headers;
		Body _body;

		ReadState _state;

		std::string _multipartBoundary;

		void envpInit(std::vector<std::string> &envp) const;
		bool parseRequestLine(const std::string &line);
		ReadState readRequestLine(BufferedReader &reader);
		ReadState readHeaderLine(BufferedReader &reader);
		ReadState readBody(BufferedReader &reader);
		ReadState readChunkedBody(BufferedReader &reader);
	};
}

#endif
