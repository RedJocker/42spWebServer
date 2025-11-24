/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constants.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 17:36:31 by maurodri          #+#    #+#             */
//   Updated: 2025/11/24 17:55:40 by maurodri         ###   ########.fr       //
/*                                                                            */
/******************************************************************************/

#ifndef CONSTANTS
# define CONSTANTS

# include <cstddef>
# include <ctime>
# include <limits>

# define DEFAULT_DOCROOT "./www"
# define DEFAULT_HOSTNAME "localhost"
# define DEFAULT_UPLOAD_FOLDER "./"
# define DEFAULT_PATH_SPEC "/**"
# define DEFAULT_PORT "8080"
# define DEFAULT_ADDRESS "0.0.0.0"
# define DEFAULT_ADDRESS_PORT DEFAULT_ADDRESS ":" DEFAULT_PORT
# define DEFAULT_LIST_DIRECTORIES false
# define DEFAULT_CGI_BIN_PATH "/usr/bin/php-cgi"
# define DEFAULT_CGI_TIMEOUT 5


const time_t CGI_TIMEOUT_NONE = std::numeric_limits<time_t>::max();
const ssize_t MAX_SIZE_BODY_UNLIMITED = -1;
#endif
