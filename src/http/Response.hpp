/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 13:22:22 by vcarrara          #+#    #+#             */
//   Updated: 2025/09/03 20:25:17 by maurodri         ###   ########.fr       //
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

			Response &setProtocol(const std::string &protocol);
			Response &setStatusCode(int code);
			Response &setStatusInfo(const std::string &info);

			Response &addHeader(const std::string &key, const std::string &value);
			Response &addHeader(const std::string &key, const size_t value);
			Response &setBody(const Body &body);
			Response &setBody(const std::string &body);
			Response &setHeaderContentLength();
			Response &setNotFound();
			Response &setBadRequest();
			Response &setOk();
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
