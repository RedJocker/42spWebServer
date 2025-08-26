// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   BufferedWriter.hpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/25 22:57:40 by maurodri          #+#    #+#             //
//   Updated: 2025/08/26 01:28:38 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef BUFFEREDWRITER_HPP
# define BUFFEREDWRITER_HPP

# include <string>

class BufferedWriter
{
public:
	enum WriteState {WRITING, DONE, ERROR};

private:
	int fd;
	WriteState state;
	std::string toWrite;
	BufferedWriter();
public:
	BufferedWriter(int fd);
	BufferedWriter(const BufferedWriter &other);
	virtual BufferedWriter &operator=(const BufferedWriter &other);
	virtual ~BufferedWriter();

	void setMessage(std::string message);
	WriteState getState() const;
	std::pair<WriteState, char*> flushMessage();

};

typedef BufferedWriter::WriteState WriteState;

#endif
