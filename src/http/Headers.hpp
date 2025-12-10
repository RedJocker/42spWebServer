/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:47:20 by vcarrara          #+#    #+#             */
/*   Updated: 2025/12/10 16:46:46 by vcarrara         ###   ########.fr       */
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
			Headers &operator=(const Headers &other);
			virtual ~Headers(void);

			struct CaseInsensitive {
				bool operator()(const std::string &s1, const std::string &s2) const;
			};

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
