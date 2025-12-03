/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scanner.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 20:03:16 by bnespoli          #+#    #+#             */
/*   Updated: 2025/12/03 14:16:26 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCANNER_HPP
#define SCANNER_HPP
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

namespace config
{
	class Scanner
	{
		
	public:
		Scanner(void);
		Scanner(const Scanner &other);
		Scanner &operator=(const Scanner &other);
		~Scanner(void);
		
		std::string content;

		size_t skipSpaces(const std::string &source, size_t initialPoint);
		int readContent(const std::string &filename);
		int readDirective(const std::string &source, size_t directiveStart, std::vector<std::string> &directives);
		const std::string &getContent(void) const;
	};
}
#endif
