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
        - Request Line
          - Method
          - Path
          - Protocol
        - Headers
        - Body
        - create a method to read a request from fd
        - create getter methods for method, path, headers, body
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
  - hello Request
    - parse a get request containing only request line
      - `GET / HTTP/1.1`
  - setup tests for http module
    - test response based on method that return response as a string
    - test request based on method that accepts fd and create a request object
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
  - create folder for connection module
  - tcp 
    - create a class to handle tcp connection
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