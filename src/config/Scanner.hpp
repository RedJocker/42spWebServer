/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scanner.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 20:03:16 by bnespoli          #+#    #+#             */
/*   Updated: 2025/11/26 20:46:14 by bnespoli         ###   ########.fr       */
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
		
	public:
		Scanner(void);
		Scanner(const Scanner &other);
		Scanner &operator=(const Scanner &other);
		~Scanner(void);
		
		std::vector<std::string> directives; // mudar depois

		int readContent(std::string filename);
		int readDirective(size_t directiveStart);
	};
}
#endif 