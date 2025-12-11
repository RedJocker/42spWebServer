// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   BufferedReader.hpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/21 21:13:23 by maurodri          #+#    #+#             //
//   Updated: 2025/12/11 05:57:24 by maurodri         ###   ########.fr       //
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

	std::string consumeBufferedContent();
	std::string consumeBufferedContent(size_t len, size_t eraseAfter);
	ssize_t crlfIndex(size_t start) const;
	void clear();
	BufferedReader();
public:
	enum ReadState {READING, DONE, ERROR, NO_CONTENT};
	BufferedReader(int fd);
	BufferedReader(const BufferedReader &toCopy);
	BufferedReader &operator=(const BufferedReader &other);
	~BufferedReader();

	std::pair<ReadState, std::string> read(size_t length);
	std::pair<ReadState, std::string> readlineCrlf(void);
	std::pair<ReadState, std::string> readAll(void);

	bool hasBufferedContent() const;
	bool bufferedContentCanMatch(const std::string &str) const;
};

typedef BufferedReader::ReadState ReadState;

#endif
