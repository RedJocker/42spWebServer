/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scanner.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 20:03:16 by bnespoli          #+#    #+#             */
//   Updated: 2025/12/03 15:37:44 by maurodri         ###   ########.fr       //
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
		std::string content;
		size_t skipSpaces(const std::string &source, size_t initialPoint);

		size_t finishReadSimpleDirective(
			const std::string &source,
			size_t directiveStart,
			size_t directiveEnd,
			std::vector<std::string> &directives);

		ssize_t finishReadCompoundDirective(
			const std::string &source,
			size_t directiveStart,
			size_t directiveCurrent,
			std::vector<std::string> &directives);
		
	public:
		Scanner(void);
		Scanner(const Scanner &other);
		Scanner &operator=(const Scanner &other);
		~Scanner(void);

		int readContent(const std::string &filename);
		ssize_t readDirective(
			const std::string &source,
			size_t directiveStart,
			std::vector<std::string> &directives);
		const std::string &getContent(void) const;
	};
}
#endif
