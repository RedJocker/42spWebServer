/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ApplicationSpec.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 16:20:44 by maurodri          #+#    #+#             */
/*   Updated: 2025/12/03 16:05:37 by bnespoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APPLICATIONSPEC_HPP
# define APPLICATIONSPEC_HPP

#include "ServerSpec.hpp"

namespace http
{
	class Application;
}

namespace config
{
	class ApplicationSpec
	{
		std::vector<ServerSpec> servers;
		
		std::vector<std::string> directives;
		
	public:
		ApplicationSpec(void);
		ApplicationSpec(const ApplicationSpec &other);
		ApplicationSpec &operator=(const ApplicationSpec &other);
		virtual ~ApplicationSpec();
		void addServer(const ServerSpec &serverSpec);
		http::Application toApplication(void);

		int applicationConfigParse(char **av);

		ssize_t isHttpDirective(const std::string &directive);
	};
}

#endif
