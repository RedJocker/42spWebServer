/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Body.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:47:30 by vcarrara          #+#    #+#             */
/*   Updated: 2025/11/04 13:55:10 by vcarrara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BODY_HPP
# define BODY_HPP

#include <string>
#include <cstddef>

namespace http {
	class Body {
		public:
			Body(void);
			Body(const Body& other);
			Body& operator=(const Body& other);
			virtual ~Body(void);

			bool parse(const char *chunk, size_t expectedLength);

			std::string str(void) const;
			void setContent(std::string content);
			void append(const char* data, size_t len);
			size_t size(void) const;
			void clear(void);

		private:
			std::string _content;
	};
}

#endif
