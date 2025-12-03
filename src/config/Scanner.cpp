/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scanner.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 20:08:11 by bnespoli          #+#    #+#             */
//   Updated: 2025/12/03 15:35:46 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "Scanner.hpp"
#include "BufferedReader.hpp"
#include <iostream>
#include "pathUtils.hpp"


namespace config {
	Scanner::Scanner(void) : content()
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

	size_t Scanner::skipSpaces(const std::string &source, size_t initialPoint)
	{
		while(initialPoint < content.size())
		{
			if (std::isspace(source[initialPoint]))
			{
				++initialPoint;
			}
			else
				return initialPoint;
		}
		return initialPoint;
	}

	int Scanner::readContent(const std::string &filename)
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
		this->content = std::string(result.second);
		std::cout << "Config file content:\n" << content << std::endl;
		delete[] result.second;
		return 0;
	}

	size_t Scanner::finishReadSimpleDirective(
		const std::string &source,
		size_t directiveStart,
		size_t directiveEnd,
		std::vector<std::string> &directives)
	{
		std::string directive = source.substr(directiveStart, directiveEnd - directiveStart + 1);
		directives.push_back(directive);
		return directiveEnd + 1;
	}

	ssize_t Scanner::finishReadCompoundDirective(
		const std::string &source,
		size_t directiveStart,
		size_t directiveCurrent,
		std::vector<std::string> &directives)
	{
		if (source.at(directiveCurrent) != '{')
			throw std::domain_error("compound directive should start with ;");
		int openBrackets = 1;
		size_t current = directiveCurrent + 1;
		while (openBrackets != 0 && current < source.size())
		{
			if (source.at(current) == '{')
				++openBrackets;
			else if (source.at(current) == '}')
				--openBrackets;
			current++;
		}

		if (openBrackets != 0)
		{
			std::cerr << "Error: unexpected end of file while reading compound directive"
					  << std::endl;
			return -1;
		} else
		{
			size_t directiveEnd = current;
			std::string directive = source.substr(directiveStart, directiveEnd - directiveStart + 1);
			directives.push_back(directive);
			return directiveEnd + 1;
		}
	}
	
	ssize_t Scanner::readDirective(
		const std::string &source, size_t directiveStart, std::vector<std::string> &directives)
	{
		// TODO dar suporte para diretivas simples e compostas

		directiveStart = this->skipSpaces(source, directiveStart);
		if (directiveStart >= source.size())
		{
			return directiveStart;
		}
		if(!std::isalpha(source[directiveStart]))
		{
			return -1;
		}
		ssize_t directiveEnd = utils::findOneOf(source, directiveStart, ";{}");
		if (directiveEnd == -1)
		{
			std::cerr << "Error: unexpected end of file." << std::endl;
			return -1;
		}
		char firstChar = source[directiveEnd];
		if (firstChar == ';')
		{
			return this->finishReadSimpleDirective(
				source, directiveStart, directiveEnd, directives);
		}
		if (firstChar == '{')
		{
			return this->finishReadCompoundDirective(
				source, directiveStart, directiveEnd, directives);
		}
		if (firstChar == '}')
		{
			std::cerr << "Error: unexpected format no starting brace" << std::endl;
			return -1;
		}
		return -1;
	}

	const std::string &Scanner::getContent(void) const
	{
		return this->content;
	}
	
} // namespace config
