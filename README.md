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
    - create class for Request
	  - [X] Request Line
		  - Method
          - Path
          - Protocol
      - Headers
      - Body
      - [X] create a method to read a request from fd
      - [X] create getter methods for method, path, headers, body
    - create class for Response
        - Status Line
          - Protocol
          - Status Code Number
          - Status Code Description
        - Headers
        - Body
        - create a method to create a response
          - maybe constructor is fine or maybe builder pattern 
        - create a method that return a response as a string
  - [X] hello Request
    - parse a get request containing only request line
      - `GET / HTTP/1.1`
  - setup tests for http module
    - test response based on method that return response as a string
    - [X] test request based on method that accepts fd and create a request object
  - get Request with headers
    - parse a get request containing some headers
      - ```
           GET / HTTP/1.1
           Connection: close
           Host: localhost
        ```
    - parse a post request with a body
      - ```
           POST / HTTP/1.1
           Connection: close
           Host: localhost
           Content-Length: 5

           hello
        ```
  - 404 response
    - create a 404 response

- Connection Handling
  - [X] create folder for connection module
  - tcp
    - [X] create a tcp connection
    - [X] create a class to handle tcp connection
	- [X] read a fixed length based message from client
	- [X] read a line message from client
	- echo a message from client
	- echo several messages from same client connection
    - handle multiple concurrent connections
    - use epoll or something alike
  - file
    - create a class to handle file opening
  - cgi
    - create a class to handle fd for inter proccess communication (IPC)
    - use epoll or something alike

- CGI
  - create folder for cgi module
  - spawn a python or php process
  - send information to process through IPC
  - receive information from process through IPC
  - research how to better deal with spawned process
    - one process only dealing with all requests concurrently
    - one process for each request
  - research how to better deal with IPC
