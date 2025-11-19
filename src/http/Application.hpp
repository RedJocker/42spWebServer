// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Application.hpp                                    :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/15 15:57:48 by maurodri          #+#    #+#             //
//   Updated: 2025/11/17 22:23:11 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef APPLICATION_HPP
# define APPLICATION_HPP

# include "Server.hpp"
# include "EventLoop.hpp"
# include <vector>

namespace http {

	class Application
	{
		std::vector<Server*> *servers;
		conn::EventLoop *eventLoop;

	public:
		Application(void);
		Application(const std::vector<Server*> &servers);
		Application(const Application &other);
		Application &operator=(const Application &other);
		virtual ~Application(void);
		int run(void);
		void shutdown(void);
	};
}

#endif
