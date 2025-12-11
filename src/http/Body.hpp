/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Body.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:47:30 by vcarrara          #+#    #+#             */
//   Updated: 2025/12/11 08:02:32 by maurodri         ###   ########.fr       //
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

		bool parse(const std::string &chunk, size_t expectedLength);

		const std::string &str(void) const;
		void setContent(const std::string &content);
		void append(const std::string &data, size_t len);
		size_t size(void) const;
		void clear(void);

	private:
		std::string _content;
	};
}

#endif
