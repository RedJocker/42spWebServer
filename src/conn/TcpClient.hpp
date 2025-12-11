// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   TcpClient.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/25 21:44:02 by maurodri          #+#    #+#             //
//   Updated: 2025/12/11 06:14:59 by maurodri         ###   ########.fr       //
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

		TcpClient();

	protected:
		BufferedReader reader;
		BufferedWriter writer;

		TcpClient(int clientFd);
		TcpClient(const TcpClient &other);
		TcpClient &operator=(const TcpClient &other);
		virtual ~TcpClient();

		std::string getMessageToSend() const;
		void setMessageToSend(std::string message);

	public:
		WriteState getWriterState() const;

		std::pair<WriteState, std::string> flushMessage();
		std::pair<WriteState, std::string> flushOperation();
		std::pair<ReadState, std::string> read(size_t length);
		std::pair<ReadState, std::string> readlineCrlf();
		std::pair<ReadState, std::string> readAllOperationFd();
		bool hasBufferedContent() const;
		int getFd() const;
	};
}

#endif
