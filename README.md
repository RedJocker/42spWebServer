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
  - [status codes](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status/)
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

## To fix

multipart upload test cases

# Sprint 2

- tests
	- multiplos servidores com portas diferentes
	- operaçoes de arquivo sem permissao de acesso ao arquivo
	- test siege com pagina em branco

 - resolver todo
	- falha de leitura no read
	- [x] olhar os construtores e destrutores e verificar inicialização de variaveis

## Sprint 1

- Http
  - file upload
    - [x] give support for 'Content-Type: multiplart/form-data; boundary ---some-bounderyadfkjla'
      - another request body format
      - has a delimiter defined as part of header that delimits body in parts
      - each body part may contain more headers
      - filename will be part of a in body header
      - browsers send this when using a form with '<input type=file>'
        - try sending a request with this format with current webserver implementation to see
          the body of request on webserver log
      - if this is part of a cgi request
        - keep body as is and send to cgi with contentType
    - [X] upload files to a folder specified by configuration
      - [x] may start with a hardcoded folder as temporary implementation
      - nginx needs an extension module for file uploads
        - [equivalent nginx extension directive](https://github.com/fdintino/nginx-upload-module?tab=readme-ov-file#upload_pass)
          - extension module accepts upload folder config on
            - `server` context (equivalent to our `http::VirtualServer`)
            - `location` context (equivalent to our `http::Route`)
  - Configurable server
    - [X] Configurable address:port
      - configuration may choose a address:port pair for server
      - [nginx similar config](https://nginx.org/en/docs/http/ngx_http_core_module.html#listen)
        - nginx allows this config only on http context (equivalent to out http::Server)
    - [X] Configurable max size of body request
      - configuration may choose a limit for body size
      - [nginx similar config](https://nginx.org/en/docs/http/ngx_http_core_module.html#client_max_body_size)
        - nginx responds with 413 Request Entity Too Large
        - nginx allows this config on
          - http contenxt (equivalent to our http::Server)
          - server context (equivalent to our virtual server to be implemented)
          - location context (equivalent to our http::Route)
      - we can also allow this config on all 3 levels and use the most specific
        - route > virtual server > server
      - [X] allow to declare max size on specs
      - [X] implement max body size checking
        - [X] start implementing a "dumb" checking
              that reads body before checking max body size
        -  [-] implement a wise check that checks max body size before reading body
            - demands some flow redesign to make early route matching
    - [X] Configurable docroot
      - configuration may choose a root server folder that maps to server file system
      - [nginx similar config](https://nginx.org/en/docs/http/ngx_http_core_module.html#root)
        - nginx allows this config on
          - `http` contenxt (equivalent to our `http::Server`)
          - `server` context (equivalent to our `http::VirtualServer`)
          - `location` context (equivalent to our `http::Route`)
    - [X] Configurable routing system
      - part of server configuration (including virtual servers)
      - [X] configurable allowed methods for a specific route
        - a certain route at '/some-route' can define only GET to be allowed, or only GET and POST, etc
      - [X] file extension based route matching
        - using /path/\*\*.ext to match files that end in .ext at /path folder or subfolders
         - using /path/\*\* to match any files at path folder or subfolder
         - using /path/*.ext to match files that end in .ext at /path folder only
         - using /path/* to match any file at /path folder only
         - using /path/specific.html to match only /path/specific.html
      - [X] configuration for directory files
        - [X] configurable "index" file (ex: index.html, index.php, any.xxx)
          - [nginx similar config](https://nginx.org/en/docs/http/ngx_http_index_module.html#index)
            - nginx allows this config on
              - `http` contenxt (equivalent to our `http::Server`)
              - `server` context (equivalent to our `http::VirtualServer`)
              - `location` context (equivalent to our `http::Route`)
            - [X] allow to declare index file
            - [X] implement checking index file
        - [X] configuration for listing directory content
          - [nginx similar config](https://nginx.org/en/docs/http/ngx_http_autoindex_module.html#autoindex)
            - ngix first tries index file if that is configured
              - nginx allows this config on
                - `http` contenxt (equivalent to our `http::Server`)
                - `server` context (equivalent to our `http::VirtualServer`)
                - `location` context (equivalent to our `http::Route`)
          - [X] allow to declare directory listing
          - [X] implement checking directory listing
        - can only have one of these behaviour at a time
          - must define behaviour if configuration file has more than one active
            - priority based is likely better option something like 'index > listing'
      - [X] configuration for default responses
        - body content of error responses like 404 or 500 may be customized
        - configuration should point to a file that will be used on response
        - [nginx similar config](https://nginx.org/en/docs/http/ngx_http_core_module.html#error_page)
          - nginx allows this config on
            - `http` contenxt (equivalent to our `http::Server`)
            - `server` context (equivalent to our `http::VirtualServer`)
            - `location` context (equivalent to our `http::Route`)
        - [X] allow to declare errorPages
        - [X] implement errorPages feature
      - [X] configurable redirection
        - a route may be configured to redirect requests to another route
          should also state what is the status code of redirection response
        - nginx has two directives related to redirection `return` and `rewrite`
          - both are allowed on `server` and `location` context
        - [X] allow to declare redirection
        - [X] implement redirection feature
      - [X] configurable file upload
        - may allow or dissalow file upload (depends on POST method being allowed)
        - may configure a folder to upload files into
      - [x] create a (interface||abstract class) Route that has at least
        - a virtual bool matches(RequestPath path, method) const
        - a virtual void serve(Client client, Monitor monitor) const
        - [x] create implementations for RouteCgi and RouteStaticFile
          - use existing code to implement serve and matches for these
            specific implementations
          - [x] create Route
          - [x] implement RouteCgi
          - [x] implement RouteStaticFile
        - route based configuration should be responsability of Route classes
  - [X] Virtual Servers
    - configuration may define serveral servers for same port, that is same tcp connection
    - each virtual server should have the same capacities as a normal server
    - on client connection it will not be possible to determine which virtual server this connection is related to
      - only after reading header 'Host: somedomainname' it will be possible to determine
        which virtual server should handle the request being parsed
    - [X] implementation: every http::Server contains a list of virtual servers with
        at leastone default virtual server the server responsability is to forward the request
        for the correct virtual server or fallback to a default if it is no possible to determine
        the right virtual server
        that is responsible to handle client request
    - http::VirtualServer configuration scope is similar to nginx `server` context
    - http::Server configuration scope is similar to nginx `http` context
    - http::Route configuration scope is similar to nginx `location` context

- CGI
  - [-] change cgi process current directory to folder of requested resource
    - use chdir on child process, after fork, before execve
      - [x] aparently php-cgi can set current directory based
        on envp SCRIPT_FILENAME
      - [ ] will need to check this again when supporting multiple cgis
      - changing chdir also has impact on relative filenames passed to cgi
        would need to do something like split filename in folder + file
        set chdir to folder and pass only filename as script
    - cgi process should be able to use relative path to acess other files
      - relative to folder of cgi resource, "folder where script is located"
  - [x] read status response from cgi
    - cgi may respond with status code through cgi response headers
    - header should be read by server, removed from headers and sent as status code
  - [x] deal with cgi unresponsiveness
    - some timeout system is required for cgi specifically, but also for all fd operations in general
        - return status 504 Gateway Timeout
          - [504 reference](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status/504#status)
  - [X] deal with cgi crashes
        - php-cgi handles runtime errors and return Status: 500 through socket pair
          so reading status from cgi response contemplates this item

- Connection
  - [-] timeout system for clients
    - client may be unresponsive after a keep-alive response
		- close
    - client may be too slow sending the whole request
    	- send response with 408 Request Timeout

- Tests
  - make end-to-end tests to test that implementation behaviour is following requirements
    - testing on 'high level', no implementation details, blackbox testing
    - set up stage enviroment so that changing dev enviroment (like ./www folder)
      does not change test results
  - make stress tests

## Sprint 0

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
      - [X] test on terminal, browsers make some url cleaning on request
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
    - [x] handle cgi ipc on EventLoop

- CGI
  - [x] create folder for cgi module
  - [x] spawn a php-cgi process
  - [X] research how to better deal with IPC
  - [x] research how to better deal with spawned process
    - one process only dealing with all requests concurrently
    - [x] one process for each request (for simplicity reasons)
  - [x] use socketpair for ipc
    - pair of connected sockets
      - each socket is bidirectional
      - child-cgi
        - [x] redirect child stdin and stdout to one side of socketpair
        - [x]close other side of socketpair
                - [x] create envp with cgi variables
                        - [x] send hardcoded values
                        - [x] send valued based on request
                - [x] call execve
                - handle error if failed execve and exit
      - server
        - [x] subscribe read/write to cgi on EventLoop
        - [x] write request body to cgi
        - [x] read cgi-response
          - [x] read a file as if it was a cgi-response with cgi-response content
          - [x] send response based on file read
          - [x] read a cgi-response from ipc to a process without processing any request input

        - [x] write full response
