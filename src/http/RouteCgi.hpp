// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteCgi.hpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 22:34:17 by maurodri          #+#    #+#             //
//   Updated: 2025/11/18 07:07:56 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef ROUTECGI_HPP
# define ROUTECGI_HPP

# include <string>
# include "Client.hpp"
# include "Operation.hpp"
# include "RouteSpec.hpp"
# include "Route.hpp"

namespace http {

	class RouteCgi : public Route
	{
		std::string cgiBinPath;
		RouteCgi();
	public:

		RouteCgi(const config::RouteSpec &routeSpec);
		RouteCgi(const RouteCgi &other);
		RouteCgi &operator=(const RouteCgi &other);
		virtual ~RouteCgi();
		virtual void serve(http::Client &client,  conn::Monitor &monitor);
		virtual void respond(http::Client &client, const Operation &operation) const;
	};

}


#endif
