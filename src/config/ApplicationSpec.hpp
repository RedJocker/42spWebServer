/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ApplicationSpec.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 16:20:44 by maurodri          #+#    #+#             */
/*   Updated: 2025/12/09 19:51:35 by bnespoli         ###   ########.fr       */
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

		std::string toString(void) const;
	};
}

#endif
