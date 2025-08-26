/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 10:41:51 by vcarrara          #+#    #+#             */
/*   Updated: 2025/08/26 13:43:16 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <map>

namespace http
{
	class Request
	{
		std::string _method;
		std::string _path;
		std::string _protocol;
		std::map<std::string, std::string> _headers;
		std::string _body;

		std::string _readBuffer; // non-blocking

	public:

		Request(void);
		Request(const Request &other);
		virtual Request &operator=(const Request &other);
		virtual ~Request(void);

		bool readFromFd(int fd); // True if populated successfully

		std::string getMethod() const;
		std::string getPath() const;
		std::string getProtocol() const;
		std::string getHeader(const std::string &key) const;
		std::string getBody() const;
	};
}

#endif
