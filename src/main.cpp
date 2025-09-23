/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcarrara <vcarrara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 17:11:02 by maurodri          #+#    #+#             */
//   Updated: 2025/09/23 19:23:07 by maurodri         ###   ########.fr       //
/*                                                                            */
/* ************************************************************************** */

#include "EventLoop.hpp"
#include <iostream>
#include "devUtil.hpp"

int main(void)
{
	conn::EventLoop eventLoop;

	http::Server server("localhost", "./www", 8080);
	std::pair<int, std::string> maybeServerFd = server.createAndListen();
	if (maybeServerFd.first < 0)
	{
		std::cout << maybeServerFd.second << std::endl;
		return 11;
	}

	eventLoop.subscribeHttpServer(&server);

	bool resLoop = eventLoop.loop();

	if (!resLoop)
		return 22;
	return 0;
}
