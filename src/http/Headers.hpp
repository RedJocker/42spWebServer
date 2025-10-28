/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:47:20 by vcarrara          #+#    #+#             */
/*   Updated: 2025/10/24 22:46:22 by maurodri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADERS_HPP
# define HEADERS_HPP

#include <string>
#include <map>

namespace http {
	class Headers {
		struct CaseInsensitive
		{
			bool operator()(const std::string &s1, const std::string &s2) const;
		};

		public:
			Headers(void);
			Headers(const Headers &other);
			Headers &operator=(const Headers &other);
			virtual ~Headers(void);

			bool parseLine(const std::string &line);

			std::string getHeader(const std::string &key) const;
			void clear(void);

			std::map<std::string, std::string, CaseInsensitive> getAll(void) const;

			bool addHeader(const std::string &key, const std::string &value);
			void eraseHeader(const std::string &key);
			std::string str(void) const;
		private:
			std::map<std::string, std::string, CaseInsensitive> _headers;
	};
}

#endif
