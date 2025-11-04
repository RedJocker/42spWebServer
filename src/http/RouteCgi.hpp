// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteCgi.hpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 22:34:17 by maurodri          #+#    #+#             //
/*   Updated: 2025/11/04 02:07:44 by maurodri         ###   ########.fr       */
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

		RouteCgi(const std::string &pathSpecification);
		RouteCgi(const RouteCgi &other);
		RouteCgi &operator=(const RouteCgi &other);
		virtual ~RouteCgi();
		virtual void serve(http::Client &client,  conn::Monitor &monitor) const;
		virtual void respond(http::Client &client, const Operation &operation) const;
	};

}


#endif
