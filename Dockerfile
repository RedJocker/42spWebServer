#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Dockerfile                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maurodri <maurodri@student.42sp...>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/11 16:08:26 by maurodri          #+#    #+#              #
#    Updated: 2025/09/11 17:15:24 by maurodri         ###   ########.fr        #
#                                                                              #
#******************************************************************************#

FROM nginx:latest

# Copy your nginx configuration
COPY nginx.conf /etc/nginx/nginx.conf

# Copy your website files
COPY www /usr/share/nginx/html

# Set proper permissions (nginx runs as user 101)
RUN chown -R 101:101 /usr/share/nginx/html && \
    chmod -R 755 /usr/share/nginx/html && \
    find /usr/share/nginx/html -type f -exec chmod 644 {} \; && \
    chmod 644 /etc/nginx/nginx.conf
