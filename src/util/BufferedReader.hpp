// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   BufferedReader.hpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/21 21:13:23 by maurodri          #+#    #+#             //
//   Updated: 2025/11/15 18:10:12 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef BUFFEREDREADER_HPP
# define BUFFEREDREADER_HPP

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

# include <vector>
# include <cstddef>
# include <unistd.h>
# include <string>

class BufferedReader
{

	int fd;
	size_t readBefore;
	std::vector<char> buffered;
	std::vector<char> saved;
	char readBuffer[BUFFER_SIZE];

	char *consumeBufferedContent();
	char *consumeBufferedContent(size_t len, size_t eraseAfter);
	ssize_t crlfIndex(size_t start) const;
	void clear();
	BufferedReader();
public:
	enum ReadState {READING, DONE, ERROR, NO_CONTENT};
	BufferedReader(int fd);
	BufferedReader(const BufferedReader &toCopy);
	BufferedReader &operator=(const BufferedReader &other);
	~BufferedReader();

	std::pair<ReadState, char *> read(size_t length);
	std::pair<ReadState, char *> readlineCrlf(void);
	std::pair<ReadState, char *> readAll(void);

	bool hasBufferedContent() const;
	bool bufferedContentCanMatch(const std::string &str) const;
};

typedef BufferedReader::ReadState ReadState;

#endif
