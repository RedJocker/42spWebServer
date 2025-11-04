// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   RouteStaticFile.hpp                                :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/10/29 22:34:17 by maurodri          #+#    #+#             //
/*   Updated: 2025/11/04 02:08:13 by maurodri         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#ifndef ROUTESTATICFILE_HPP
# define ROUTESTATICFILE_HPP

# include <string>
# include "Route.hpp"
# include "Client.hpp"
# include "Operation.hpp"

namespace http {

	class RouteStaticFile : public Route
	{
		RouteStaticFile();

		void handleGetFile(http::Client &client, conn::Monitor &monitor) const;
		void handleGetDirectory(http::Client &client, conn::Monitor &monitor) const;
		void handlePost(http::Client &client, conn::Monitor &monitor) const;
		void handleDelete(http::Client &client, conn::Monitor &monitor) const;

		void onFileRead(http::Client &client, const std::string &fileContent) const;
		void onFileWritten(http::Client &client) const;

	public:

		RouteStaticFile(std::string path);
		RouteStaticFile(const RouteStaticFile &other);
		RouteStaticFile &operator=(const RouteStaticFile &other);
		virtual ~RouteStaticFile();
		virtual void serve(http::Client &client,  conn::Monitor &monitor) const;
		virtual void respond(http::Client &client, const Operation &operation) const;
	};
}

#endif
