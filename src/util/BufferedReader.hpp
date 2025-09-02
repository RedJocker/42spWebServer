// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   BufferedReader.hpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/21 21:13:23 by maurodri          #+#    #+#             //
//   Updated: 2025/08/29 00:16:40 by maurodri         ###   ########.fr       //
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
# include <unistd.h>

class BufferedReader
{

	int fd;
	size_t readBefore;
	std::vector<char> buffered;
	char readBuffer[BUFFER_SIZE];

	char *consumeBufferedContent();
	char *consumeBufferedContent(size_t len, size_t eraseAfter);
	ssize_t crlfIndex(size_t start) const;
	BufferedReader();
public:
	enum ReadState {READING, DONE, ERROR, NO_CONTENT};
	BufferedReader(int fd);
	BufferedReader(const BufferedReader &toCopy);
	BufferedReader &operator=(const BufferedReader &other);
	~BufferedReader();

	std::pair<ReadState, char *> read(size_t length);
	std::pair<ReadState, char *> readlineCrlf();
	bool hasBufferedContent() const;
};

typedef BufferedReader::ReadState ReadState;

#endif
