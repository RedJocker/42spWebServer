# References:

- git
  - [semantic commits](https://gist.github.com/joshbuchea/6f47e86d2510bce28f8e7f42ae84c716)
  - [basic branch workflow](https://br.eheidi.dev/git-github/04-branch-e-pull-request/)

- nginx
  - [docs](https://nginx.org/en/docs/) 
  - [nginx confic syntax](https://docs.nginx.com/nginx/admin-guide/basic-functionality/managing-configuration-files/)
  - [http core module reference](https://nginx.org/en/docs/http/ngx_http_core_module.html)
  - [core module reference](https://nginx.org/en/docs/ngx_core_module.html)

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
    test based on method that return response as a string
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
