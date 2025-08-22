// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   BufferedReader.hpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/21 21:13:23 by maurodri          #+#    #+#             //
//   Updated: 2025/08/22 01:43:03 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef BUFFEREDREADER_HPP
# define BUFFEREDREADER_HPP

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

# include <vector>
# include <utility>
# include <cstddef>

class BufferedReader
{

	int fd;
	int readBefore;
	std::vector<char> buffered;
	char readBuffer[BUFFER_SIZE];

	BufferedReader();
public:
	enum ReadState {READING, DONE, ERROR};
	BufferedReader(int fd);
	BufferedReader(BufferedReader &toCopy);
	BufferedReader operator=(BufferedReader &other);
	~BufferedReader();

	std::pair<ReadState, char *> read(size_t length);
};

typedef BufferedReader::ReadState ReadState;

#endif
