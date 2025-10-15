/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 13:22:22 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/15 14:21:35 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <unistd.h>
#include "Headers.hpp"
#include "Body.hpp"

namespace http {
	class Response {
		public:
			Response(void);
			Response(const std::string &protocol, int statusCode, const std::string &statusInfo);
			Response(const Response &other);
			Response &operator=(const Response &other);
			virtual ~Response(void);

			std::string getHeader(const std::string &key) const;
			Headers &headers();

			Response &setProtocol(const std::string &protocol);
			Response &setStatusCode(int code);
			Response &setStatusInfo(const std::string &info);

			Response &addHeader(const std::string &key, const std::string &value);
			Response &addHeader(const std::string &key, const size_t value);
			Response &setBody(const Body &body);
			Response &setBody(const std::string &body);
			Response &setHeaderContentLength();

			Response &setCreated();
			Response &setOk();
			Response &setNotFound();
			Response &setImTeapot();
			Response &setBadRequest();
			Response &setInternalServerError();

			void clear(void);

			std::string toString(void) const;

			void setFileBody(int fd);
			bool isStreaming(void) const;
			int getFileFd(void) const;
			size_t getBytesSent(void) const;
			void addBytesSent(size_t n);
			void closeFile(void);

		private:
			std::string _protocol;
			int _statusCode;
			std::string _statusInfo;

			Headers _headers;
			Body _body;

			int _fileFd;
			size_t _bytesSent;
			bool _isStreaming;
	};
}

#endif
