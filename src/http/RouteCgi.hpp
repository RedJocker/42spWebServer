// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteCgi.hpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 22:34:17 by maurodri          #+#    #+#             //
//   Updated: 2025/10/29 23:48:51 by maurodri         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef ROUTECGI_HPP
# define ROUTECGI_HPP

# include <string>
# include "Route.hpp"
# include "Client.hpp"

namespace http {

	class RouteCgi : public Route
	{

		std::string path;
		RouteCgi();
	public:

		RouteCgi(std::string path);
		RouteCgi(const RouteCgi &other);
		RouteCgi &operator=(const RouteCgi &other);
		virtual ~RouteCgi();
		virtual bool matches(const RequestPath &path, const std::string &method) const;
		virtual void serve(http::Client &client,  conn::Monitor &monitor) const;
		virtual void respond(http::Client &client, const std::string &operationContent) const;
	};

}


#endif
