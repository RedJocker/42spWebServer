/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Body.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:47:30 by vcarrara          #+#    #+#             */
/*   Updated: 2025/09/02 11:44:53 by vcarrara         ###   ########.fr       */
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
			virtual Body& operator=(const Body& other);
			virtual ~Body(void);

			bool parse(const char *chunk, size_t expectedLength);

			void setContent(const std::string &content);

			std::string str(void) const;
			size_t size(void) const;

		private:
			std::string _content;
	};
}

#endif
