# References:

- git
  - [semantic commits](https://gist.github.com/joshbuchea/6f47e86d2510bce28f8e7f42ae84c716)
  - [basic branch workflow](https://br.eheidi.dev/git-github/04-branch-e-pull-request/)

- nginx
  - [docs](https://nginx.org/en/docs/)
  - [nginx config syntax](https://docs.nginx.com/nginx/admin-guide/basic-functionality/managing-configuration-files/)
  - [http core module reference](https://nginx.org/en/docs/http/ngx_http_core_module.html)
  - [core module reference](https://nginx.org/en/docs/ngx_core_module.html)

- http
  - [reference RFC](https://www.rfc-editor.org/rfc/rfc2616)
  - [http overview mozilla](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Overview)
  - [crlf](https://stackoverflow.com/questions/1552749/difference-between-cr-lf-lf-and-cr-line-break-types)

- cgi
  - [reference RFC](https://www.rfc-editor.org/rfc/rfc3875)
  - [some examples in different languages](https://gist.github.com/stokito/a9a2732ffc7982978a16e40e8d063c8f)

- tcp
  - [create a tcp connection](https://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/)
  - reuse address
    - [How do I use setsockopt(SO_REUSEADDR)](https://stackoverflow.com/questions/24194961/how-do-i-use-setsockoptso-reuseaddr)
    - [Understanding INADDR_ANY](https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming)
    - [setsockopt](https://linux.die.net/man/3/setsockopt)
  - [epool example](https://gist.github.com/kesuskim/4624a52146cc1e2676fa0434cef17281)
  - [epool intro](https://www.suchprogramming.com/epoll-in-3-easy-steps/)
  - [epool master](https://thelinuxcode.com/epoll-7-c-function/)

# Tasks

- [X] init repository

- Configuration File
  - create folder for configuration file module
  - hello configuration
    - read something from a fd and make some assertion on content that was read
  - setup tests for configuration file module
  - trivial configuration
    - take a basic trivial nginx config and parse it
      ```
                http {
                    # Configuration specific to HTTP and affecting all virtual servers

                    server {
                        # configuration of HTTP virtual server 1
                        location /one {
                            # configuration for processing URIs starting with '/one'
                        }
                        location /two {
                            # configuration for processing URIs starting with '/two'
                        }
                    }

                    server {
                        # configuration of HTTP virtual server 2
                    }
                }
      ```
    - create a public interface for reading parsed configuration

- Http
  - [X] create folder for http module
  - end goal
    - [X] create class for Request
      - [X] Request Line
        - [X] Method
        - [X] Path
        - [X] Protocol
      - [X] Headers
      - [X] Body
      - [X] create a method to read a request from fd
      - [X] create getter methods for method, path, headers, body
    - [X] create class for Response
        - [X] Status Line
          - [X] Protocol
          - [X] Status Code Number
          - [X] Status Code Description
        - [X] Headers
        - [X] Body
        - [X] create a method to create a response
          - [X] maybe constructor is fine or maybe builder pattern
        - [X] create a method that return a response as a string
  - [X] create a class for Body to be able to use on both Request and Response
  - [X] create a class for Headers to be able to use on both Request and Response
    - [X] maybe it is a good idea to inherit from std::map<std::string, std::string>
  - [X] change parse Request, do not read from fd, read from TcpClient instead
    - [X] change readFromfd(int fd) to readFromTcpClient(TcpClient client)
      - [X] use client.readlineCrlf to read a \r\n delimited line
      - [X] use client.read(len) to read a fixed size content for body parsing
        - [X] length is part of headers
      - [X] create a enum for request read state
        - [X] {READING_REQUEST_LINE, READING_HEADERS, READING_BODY, ERROR, DONE}
      - [X] return this request read state from readFromTcpClient
      - [X] it may be a good idea to delegate to Header class responsabillity to parse headers
      - [X] it may be a good idea to delegate to Body class responsability to parse body
    - [X] remember we cannot by requirement check errno on read and writes to socket
      - [X] do not use EAGAIN nor EWOULDBLOCK
  - [X] hello Request
    - [X] parse a get request containing only request line
      - [X] `GET / HTTP/1.1`
  - setup tests for http module
    - [ ] test response based on method that return response as a string
    - [X] test request based on method that accepts fd and create a request object
  - [X] get Request with headers
    - [X] parse a get request containing some headers
      - ```
           GET / HTTP/1.1
           Connection: close
           Host: localhost
        ```
    - [X] parse a post request with a body
      - ```
           POST / HTTP/1.1
           Connection: close
           Host: localhost
           Content-Length: 5

           hello
        ```
  - [X] 404 response
    - [X] create a 404 response
  - [X] create a HttpClient class that inherits from TcpClient and has a Request and Response
    - [X] Request and Response are initially empty
    - [X] we parse Request using Request.readFromTcpClient()
    - [X] generate some Response and send it back to client
    - [X] next message from client overwrites previous Request
      - [X] maybe we will need to clear first before reusing
    - [X] provide a public method to return the state of parsing and the current request
  - [X] exchange message browser and respond 404
  - [X] detect and respond TRACE http method requests
    - [X] response body must have the request in literal form
    - [X] debugging purposes
    - [X] helps start developing routing by reacting to a request with a response production
      - [X] at some point we will need to be able, following configuration:
        - [X] to run multiple servers
        - [X] each server having their own internal routing
        - [X] support for some routes processing their request through cgi on response production
      - [X] create a class Dispatcher
        - [X] whith a method http::Response dispatch(http::Client &client)
        - [X] read client.request.method and if == trace
            - [X] respond 200 Ok with body = client.request.asString()
            - [X] don't forget to add Content-Length header on response
  - [X] respond a GET request to an existing file
    - [X] if found respond 200 OK with the body containing the file requested on body
    - [X] assume it is a found content file for now, later deal with 404 and directory listing
    - [X] assume only one server for now
    - [X] docroot
      - [X] folder that is associated with path
      - [X] / on path is bind to some folder on computer
        - [X] use hardcoded folder on project for now
        - [X] eventually each server has its own docroot defined on configuration file
    - [X] make a blocking implementation, we will solve the problem of integrating with epool later
  - [X] respond a POST request by writting the body into a file
    - [X] if success respond 201 Created
    - [X] use path as filepath
      - [X] create file if not exists
      - [X] for now assume folder exists
      - [X] take docroot as base folder
      - [X] lets deal with relative paths later
    - [X] assume only one server for now
    - [X] make a blocking implementation, we will solve the problem of integrating with EventLoop later
  - [X] respond a DELETE request by deleting a file resource
    - [X] if success respond 204 No Content
    - [X] use path as filepath
    - [X] assume only one server for now
    - [X] make a blocking implementation, we will solve the problem of integrating with EventLoop
  - [X] respond a GET to directory listing contents
    - [X] if success respond 200 Ok
    - [X] on body send a generated html all files listed as links
      - [X] check nginx response for that case
    - [X] assume our config enable directory listing
    - [X] use path as filepath
    - [X] assume only one server for now
    - [X] make a blocking implementation, we will solve the problem of integrating with EventLoop later
  - [X] include a route for cgi that will integrate with cgi code
    - [X] we can start by
      - [X] checking if is exactly index.php before other static files
      - [X] responding something
        - [X] maybe 418 I'm a teapot just to know it is our response
  - [X] Handle relative url path
    - [X] it should be possible to use relative path on url
    - [X] create subdirectories on www to test relative path works
    - [X] it should not be possible to go outside docroot
      - [X] `GET /../Makefile HTTP/1.1` should return 404 not found
      - [ ] test on terminal, browsers make some url cleaning on request
  - [X] Handle header Connection: close
  - [X] create a http::Server that is a conn::TcpServer
    - [X] this class will be responsible for server specific behaviour
    - [X] will enable running multiple servers concurrently
    - [X] docroot
    - [X] hostname
    - [X] port
    - [X] routing
    - [X] Dispatcher will forward request handling to http:Server if it is one of valid methods
  - [X] Handle header Transfer-Encoding: chunked
    - [X] a slightly different protocol for reading body
      - [X] size1\r\ncontent1\r\nsize2\r\ncontent2\r\n0\r\n

- Connection Handling
  - [X] create folder for connection module
  - tcp
    - [X] create a tcp connection
    - [X] create a class to handle tcp connection
    - [X] read a fixed length based message from client
    - [X] read a line message from client
    - [X] echo a message from client
    - [X] echo several messages from same client connection
    - [X] handle multiple concurrent connections
    - [X] use epoll or something alike
    - handle todos left on EventLoop
    - [X] fix issues of buffered reader: a read may contain several \r\n in a single read
    - [X] Reformat EventLoop
      - change IO Multiplexing from epoll to poll
        - epoll does not work with regular files
    - subject states that
      - all reads and writes except configuration file should happen through poll (multiplexer)
        - this includes reading regular files while responding to requests
        - poll states that it does not make sense to monitor regular files fds
          - select and poll always consider regular files ready to read and write
            - this is related to in-kernel buffering of io to disk
          - but it is still possible to monitor regular files with poll
         - epoll does not allow monitoring regular files
    - [X] handle file reading on EventLoop
    - [X] handle file writing on EventLoop
    - handle cgi ipc on EventLoop

- CGI
  - create folder for cgi module
  - spawn a php-cgi process
  - [X] research how to better deal with IPC
  - research how to better deal with spawned process
    - one process only dealing with all requests concurrently
    - one process for each request
  - use socketpair for ipc
    - pair of connected sockets
      - each socket is bidirectional
      - child-cgi
        - [x] redirect child stdin and stdout to one side of socketpair
        - [x]close other side of socketpair
		- [ ] create envp with cgi variables
			- [x] send hardcoded values
			- send valued based on request
		- [x] call execve
		- handle error if failed execve and exit
      - server
        - subscribe read/write to cgi on EventLoop
        - write request body to cgi
        - [x] read cgi-response
	  - [x] read a file as if it was a cgi-response with cgi-response content
	  - [x] send response based on file read
	  - [x] read a cgi-response from ipc to a process without processing any request input

        - write full response
