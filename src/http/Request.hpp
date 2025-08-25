/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 10:41:51 by vcarrara          #+#    #+#             */
//   Updated: 2025/08/25 16:44:29 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>

namespace http
{
	class Request
	{
	private:
		std::string _method;
		std::string _path;
		std::string _protocol;

	public:
		Request(void);
		Request(const Request &other);
		virtual Request &operator=(const Request &other);
		virtual ~Request();

		bool parseFromFd(int fd); // True if populated successfully

		std::string getMethod() const;
		std::string getPath() const;
		std::string getProtocol() const;
	};
}

#endif
