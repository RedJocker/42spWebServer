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
  - create folder for http module
  - end goal
    - [X] create class for Request
      - [X] Request Line
        - Method
        - Path
        - Protocol
      - [X] Headers
      - [X] Body
      - [X] create a method to read a request from fd
      - [X] create getter methods for method, path, headers, body
    - [X] create class for Response
        - Status Line
          - Protocol
          - Status Code Number
          - Status Code Description
        - Headers
        - Body
        - create a method to create a response
          - maybe constructor is fine or maybe builder pattern 
        - create a method that return a response as a string
  - [X] create a class for Body to be able to use on both Request and Response
  - [X] create a class for Headers to be able to use on both Request and Response
    - maybe it is a good idea to inherit from std::map<std::string, std::string>
  - [X] change parse Request, do not read from fd, read from TcpClient instead
    - change readFromfd(int fd) to readFromTcpClient(TcpClient client)
      - use client.readlineCrlf to read a \r\n delimited line
      - use client.read(len) to read a fixed size content for body parsing
        - length is part of headers
      - create a enum for request read state
        - {READING_REQUEST_LINE, READING_HEADERS, READING_BODY, ERROR, DONE}
      - return this request read state from readFromTcpClient
      - it may be a good idea to delegate to Header class responsabillity to parse headers
      - it may be a good idea to delegate to Body class responsability to parse body
    - remember we cannot by requirement check errno on read and writes to socket
      - do not use EAGAIN nor EWOULDBLOCK
  - [X] hello Request
    - parse a get request containing only request line
      - `GET / HTTP/1.1`
  - setup tests for http module
    - test response based on method that return response as a string
    - [X] test request based on method that accepts fd and create a request object
  - [X] get Request with headers
    - parse a get request containing some headers
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
    - create a 404 response
  - [X] create a HttpClient class that inherits from TcpClient and has a Request and Response
    - Request and Response are initially empty
    - we parse Request using Request.readFromTcpClient()
    - generate some Response and send it back to client
    - next message from client overwrites previous Request
      - maybe we will need to clear first before reusing
    - provide a public method to return the state of parsing and the current request
  - [X] exchange message browser and respond 404
  - [X] detect and respond TRACE http method requests
    - response body must have the request in literal form
    - debugging purposes
    - helps start developing routing by reacting to a request with a response production
      - at some point we will need to be able, following configuration:
        - to run multiple servers
        - each server having their own internal routing
        - support for some routes processing their request through cgi on response production
      - create a class Dispatcher
        - whith a method http::Response dispatch(http::Client &client)
        - read client.request.method and if == trace
            - respond 200 Ok with body = client.request.asString()
            - don't forget to add Content-Length header on response
  - [X] respond a GET request to an existing file
    - if found respond 200 OK with the body containing the file requested on body
    - assume it is a found content file for now, later deal with 404 and directory listing
    - assume only one server for now
    - docroot
      - folder that is associated with path
      - / on path is bind to some folder on computer
        - use hardcoded folder on project for now
        - eventually each server has its own docroot defined on configuration file
    - make a blocking implementation, we will solve the problem of integrating with epool later
 - respond a POST request by writting the body into a file
    - if success respond 201 Created
    - use path as filepath
      - create file if not exists
      - for now assume folder exists
      - take docroot as base folder
      - lets deal with relative paths later
    - assume only one server for now
    - make a blocking implementation, we will solve the problem of integrating with EventLoop later
 - respond a DELETE request by deleting a file resource
    - if success respond 204 No Content
    - use path as filepath
    - assume only one server for now
    - make a blocking implementation, we will solve the problem of integrating with EventLoop
 - respond a GET to directory listing contents
    - if success respond 200 Ok
    - on body send a generated html all files listed as links
      - check nginx response for that case
    - assume our config enable directory listing
    - use path as filepath
    - assume only one server for now
    - make a blocking implementation, we will solve the problem of integrating with EventLoop later
  - include a route for cgi that will integrate with cgi code
    - we can start by
      - checking if is exactly index.php before other static files
      - responding something
        - maybe 418 I'm a teapot just to know it is our response
  - Handle header Connection: close
  - create a http::Server that is a conn::TcpServer
    - this class will be responsible for server specific behaviour
    - will enable running multiple servers concurrently
    - docroot
    - hostname
    - port
    - routing
    - Dispatcher will forward request handling to http:Server if it is one of valid methods
  - Handle header Transfer-Encoding: chunked
    - a slightly different protocol for reading body
      - size1\r\ncontent1\r\nsize2\r\ncontent2\r\n0\r\n
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
    - [ ] handle file writing on EventLoop
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
        - redirect child stdin and stdout to one side of socketpair
        - close other side of socketpair
      - server
        - subscribe read/write to cgi on EventLoop
        - write body
        - read cgi-response
        - write full response
