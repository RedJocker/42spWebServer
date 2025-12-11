/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Body.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:47:36 by vcarrara          #+#    #+#             */
//   Updated: 2025/12/11 08:00:58 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Body.hpp"
#include <cstring>

namespace http {
	Body::Body(void) : _content() {}
	Body::Body(const Body& other) {
		this->_content = other._content;
	}
	Body& Body::operator=(const Body& other) {
		if (this != &other)
			this->_content = other._content;
		return *this;
	}
	Body::~Body(void) {}

	// Append chunk to body and ensures no overflow
	bool Body::parse(const std::string &chunk, size_t expectedLength) {
		_content.append(chunk);
		if (_content.size() > expectedLength) {
			_content.resize(expectedLength);
		}

		return true;
	}

	void Body::setContent(const std::string &content)
	{
		this->_content = content;
	}

	void Body::append(const std::string &data, size_t len) {
		_content.append(data, len);
	}

	// Return content as string
	const std::string &Body::str(void) const {
		return _content;
	}

	// Return content size
	size_t Body::size(void) const {
		return _content.size();
	}

	void Body::clear(void) {
		_content = "";
	}
}
