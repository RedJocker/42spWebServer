/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:47:20 by vcarrara          #+#    #+#             */
/*   Updated: 2025/09/02 11:40:06 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADERS_HPP
# define HEADERS_HPP

#include <string>
#include <map>

namespace http {
	class Headers {
		public:
			Headers(void);
			Headers(const Headers &other);
			virtual Headers &operator=(const Headers &other);
			virtual ~Headers(void);

			bool parseLine(const std::string &line);

			std::string getHeader(const std::string &key) const;

			void setHeader(const std::string &key, const std::string &value);
			bool has(const std::string &key) const;
			std::string toString(void) const;

		private:
			std::map<std::string, std::string> _headers;
	};
}

#endif
