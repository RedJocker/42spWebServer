/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scanner.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 20:08:11 by bnespoli          #+#    #+#             */
/*   Updated: 2025/11/26 20:16:07 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Scanner.hpp"
#include "BufferedReader.hpp"
#include <iostream>


namespace config {
	Scanner::Scanner(void) : content(NULL)
	{
	}

	Scanner::Scanner(const Scanner &other)
	{
		*this = other;
	}

	Scanner &Scanner::operator=(const Scanner &other)
	{
		if (this == &other)
			return *this;
		this->content = other.content;
		return *this;
	}

	Scanner::~Scanner(void)
	{
	}

	int Scanner::readContent(std::string filename)
	{
		int fd = ::open(filename.c_str(), O_RDONLY, 0666);

		if (fd == -1)
		{
			std::cerr << "Error opening config file: " << filename << std::endl;
			return 1;
		}
		BufferedReader reader(fd);
		std::cout << "Parsing config file: " << filename << std::endl;
		std::pair<ReadState, char *> result;
		while (true)
		{
			std::cout << "Reading..." << std::endl;
			result = reader.readAll();
			std::cout << "Read state: " << result.first << std::endl;
			if (result.first == BufferedReader::ERROR)
			{
				std::cerr << "Error reading config file: " << result.second << std::endl;
				return 1;
			}
			else if (result.first == BufferedReader::NO_CONTENT || result.first == BufferedReader::DONE)
				break;
		}

		if (result.first == BufferedReader::DONE)
		{
			std::cerr << "unexpected path: " << result.second << std::endl;
			return 1;
		}
		char *content = result.second;
		std::cout << "Config file content:\n" << content << std::endl;
		delete[] content;
		return 0;
	}
} // namespace config