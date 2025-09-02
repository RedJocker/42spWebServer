/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 13:22:22 by vcarrara          #+#    #+#             */
/*   Updated: 2025/09/02 15:27:52 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include "Headers.hpp"
#include "Body.hpp"

namespace http {
	class Response {
		public:
			Response(void);
			Response(const std::string &protocol, int statusCode, const std::string &statusInfo);
			Response(const Response &other);
			virtual Response &operator=(const Response &other);
			virtual ~Response(void);

			void setProtocol(const std::string &protocol);
			void setStatusCode(int code);
			void setStatusInfo(const std::string &info);

			void setBody(const std::string &body);
			Body getBody(void) const;

			void addHeader(const std::string &key, const std::string &value);
			void clear(void);

			std::string toString(void) const;

		private:
			std::string _protocol;
			int _statusCode;
			std::string _statusInfo;

			Headers _headers;
			Body _body;
	};
}

#endif
