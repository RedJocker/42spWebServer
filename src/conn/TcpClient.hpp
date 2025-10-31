// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   TcpClient.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/08/25 21:44:02 by maurodri          #+#    #+#             //
//   Updated: 2025/10/28 23:43:42 by maurodri         ###   ########.fr       //
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
		int operationFd;
		pid_t cgiPid;

		TcpClient();

	protected:
		BufferedReader reader;
		BufferedWriter writer;

		TcpClient(int clientFd);
		TcpClient(const TcpClient &other);
		TcpClient &operator=(const TcpClient &other);
		virtual ~TcpClient();

		std::string getMessageToSend() const;

	public:
		WriteState getWriterState() const;
		void setMessageToSend(std::string message);
		std::pair<WriteState, char*> flushMessage();
		std::pair<WriteState, char*> flushOperation();
		std::pair<ReadState, char*> read(size_t length);
		std::pair<ReadState, char*> readlineCrlf();
		std::pair<ReadState, char*> readAllOperationFd();
		bool hasBufferedContent() const;
		int getFd() const;
		int getOperationFd() const;
		pid_t getCgiPid(void) const;
		void setOperationFd(int operationFd);
		void setOperationFd(int operationFd, std::string writeContent);
		void setCgiPid(pid_t cgiPid);
		void clearReadOperation();
		void clearWriteOperation();
		void clearCgiPid(void);
	};
}

#endif
