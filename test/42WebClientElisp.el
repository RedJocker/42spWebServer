;******************************************************************************;
;                                                                              ;
;                                                         :::      ::::::::    ;
;    42WebClientElisp.el                                :+:      :+:    :+:    ;
;                                                     +:+ +:+         +:+      ;
;    By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+         ;
;                                                 +#+#+#+#+#+   +#+            ;
;    Created: 2025/10/06 19:13:04 by maurodri          #+#    #+#              ;
;    Updated: 2025/10/07 00:39:50 by maurodri         ###   ########.fr        ;
;                                                                              ;
;******************************************************************************;

; local variable buffer = new buffer
(setq-local buffer (get-buffer-create "*42webclient*")) 

; scoped variable connection = new tcp connection
(setq-local connection (open-network-stream 
       "42webclient"
       buffer
       "localhost"
       8080
       :type 'plain))
			
(defun httpGet (conn path)
  "Function  that makes a get request on path using conn"
  (process-send-string
   conn
   (format "GET %s HTTP/1.1\r\nHost: localhost\r\n\r\n" path)))

(defun httpPost (conn path body)
  "Function  that makes a post request on path with body using conn"
  (process-send-string
   conn
   (format "POST %s HTTP/1.1\r\n\
Host: localhost\r\n\
Content-Length: %d\r\n\
\r\n%s" path (length body) body)))

(defun httpDelete (conn path)
  "Function  that makes a delete request on path using conn"
  (process-send-string
   conn
   (format "DELETE %s HTTP/1.1\r\nHost: localhost\r\n\r\n" path)))

(httpGet connection "/")
(httpPost connection "/todo.cgi" "hello there")
(httpGet connection "/todo.cgi")
(httpGet connection "/todo.cgi?hello=there&def=xhy")
(httpDelete connection "/hello.txt")
(httpGet connection "/hello.txt")
(httpPost connection "/hello.txt" "content of hello there")
(httpGet connection "/42")
(httpGet connection "/42/")
(httpGet connection "/42/..")
(httpGet connection "/42/../42")
(httpGet connection "/42/index.html")
(httpGet connection "/exit")
(delete-process connection) ; close tcp connection

