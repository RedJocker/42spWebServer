// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   TcpClient.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/25 21:44:02 by maurodri          #+#    #+#             //
//   Updated: 2025/08/26 21:30:12 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef TCPCLIENT_HPP
# define TCPCLIENT_HPP

# include "BufferedReader.hpp"
# include "BufferedWriter.hpp"

namespace conn {
	class TcpClient
	{
		int clientFd;
		BufferedReader reader;
		BufferedWriter writer;
		TcpClient();

	public:
		TcpClient(int clientFd);
		TcpClient(const TcpClient &other);
		virtual TcpClient &operator=(const TcpClient &other);
		virtual ~TcpClient();

		std::pair<ReadState, char *> read(size_t length);
		std::pair<ReadState, char *> readlineCrlf();
		void setMessageToSend(std::string message);
		WriteState getWriterState() const;
		std::string getMessageToSend() const;
		std::pair<WriteState, char*> flushMessage();
	};
}

#endif
