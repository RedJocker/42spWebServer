/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 11:21:56 by vcarrara          #+#    #+#             */
/*   Updated: 2025/09/02 12:24:53 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include "Headers.hpp"
#include "Body.hpp"

namespace http {
	class Response {
		private:
			std::string _protocol;
			int _statusCode;
			std::string _statusInfo;
			Headers _headers;
			Body _body;

		public:
			Response(void);
			Response(const std::string &protocol, int statusCode, const std::string &statusInfo);
			Response(const Response &other);
			virtual Response &operator=(const Response &other);
			virtual ~Response(void);

			static Response NotFound(void);

			void setProtocol(const std::string &protocol);
			void setStatusCode(int code);
			void setStatusInfo(const std::string &info);
			void setHeader(const std::string &key, const std::string &value);
			void setBody(const std::string &body);

			std::string getProtocol(void) const;
			int getStatusCode(void) const;
			std::string getStatusInfo(void) const;
			const Headers &getHeaders(void) const;
			const Body &getBody(void) const;

			std::string toString(void) const;
	};
}

#endif
