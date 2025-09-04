/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:47:20 by vcarrara          #+#    #+#             */
//   Updated: 2025/09/03 20:22:39 by maurodri         ###   ########.fr       //
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
			void clear(void);

			std::map<std::string, std::string> getAll(void) const;

			bool addHeader(const std::string &key, const std::string &value);
			std::string str(void) const;
		private:
			std::map<std::string, std::string> _headers;
	};
}

#endif
