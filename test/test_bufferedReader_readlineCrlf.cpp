// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   test_bufferedReader_readlineCrlf.cpp               :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/22 20:07:04 by maurodri          #+#    #+#             //
//   Updated: 2025/08/22 21:16:15 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "BufferedReader.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	if (argc != 3)
		return 43;

	int fd = open(argv[1], O_RDONLY, 644);
	BufferedReader reader(fd);

	int linesToRead = ::atoi(argv[2]);

	for (int i = 0; i < linesToRead; i++)
	{
		std::pair<ReadState, char *> readerResult;
		while (readerResult.first == BufferedReader::READING)
		{
			std::cout << "reading" << std::endl;
			readerResult = reader.readlineCrlf();
		}
		if (readerResult.first == BufferedReader::DONE)
		{
			std::cout << "done: " << readerResult.second << std::endl;
		}
		else if (readerResult.first == BufferedReader::NO_CONTENT)
		{
			std::cout << "eof: " << readerResult.second << std::endl;
		}
		else if (readerResult.first == BufferedReader::ERROR)
		{
			return 22;
		}
	}

	return 0;
}
