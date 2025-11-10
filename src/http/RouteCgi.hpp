// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteCgi.hpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 22:34:17 by maurodri          #+#    #+#             //
//   Updated: 2025/11/09 13:07:52 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef ROUTECGI_HPP
# define ROUTECGI_HPP

# include <string>
# include "Route.hpp"
# include "Client.hpp"
# include "Operation.hpp"

namespace http {

	class RouteCgi : public Route
	{
		RouteCgi();
	public:

		RouteCgi(
			const std::string &pathSpecification,
			const std::string &docroot,
			const std::vector<std::string> &allowedMethods);
		RouteCgi(const RouteCgi &other);
		RouteCgi &operator=(const RouteCgi &other);
		virtual ~RouteCgi();
		virtual void serve(http::Client &client,  conn::Monitor &monitor);
		virtual void respond(http::Client &client, const Operation &operation) const;
	};

}


#endif
