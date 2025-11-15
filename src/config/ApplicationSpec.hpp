// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ApplicationSpec.hpp                                :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri </var/mail/maurodri>              +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/11/15 16:20:44 by maurodri          #+#    #+#             //
//   Updated: 2025/11/15 17:07:22 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

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
	public:
		ApplicationSpec(void);
		ApplicationSpec(const ApplicationSpec &other);
		ApplicationSpec &operator=(const ApplicationSpec &other);
		virtual ~ApplicationSpec();
		void addServer(const ServerSpec &serverSpec);
		http::Application toApplication(void);
	};
}

#endif
