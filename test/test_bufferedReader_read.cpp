// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   test_bufferedReader_read.cpp                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/22 00:47:23 by maurodri          #+#    #+#             //
//   Updated: 2025/08/22 19:56:38 by maurodri         ###   ########.fr       //
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

	int toRead = ::atoi(argv[2]);

	std::pair<ReadState, char *> readerResult;
	while (readerResult.first == BufferedReader::READING)
	{
		std::cout << "reading" << std::endl;
		readerResult = reader.read(toRead);
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

	return 0;
}
