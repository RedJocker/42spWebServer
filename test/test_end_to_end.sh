#!/bin/bash

actual_basedir=$(basename $(pwd))
expected_basedir='webserver'
expected_basedir_regex=".*$expected_basedir$"

if ! [[ $actual_basedir =~ $expected_basedir_regex ]]; then
    echo 'execute test on base dir'
    echo "current directory $actual_basedir"
    echo "expected directory ending with $expected_basedir"
    echo 'you may change expected_basedir on test script if basedir has a different name'
    exit 42
fi

make test_end_to_end
build_status="$?"

if (( build_status != 0 )); then
    exit $build_status;
fi

port=7000

setup_server() {
    logfile=$(mktemp)
    port=$(( ++port ))
    ./test_end_to_end $1 "localhost:$port" > "$logfile" 2>&1 &
    SERVER_PID=$!
    sleep 0.1
}

teardown_server() {
    kill -2 $SERVER_PID 2> /dev/null
    wait $SERVER_PID
}

test_connection() {
    echo 'test_connection:'" $1"
    setup_server "$1"
    server_output_logfile=$(echo "$logfile")
    nc -v -z -w 2 "$2" "$port"
    nc_exit_status=$?
    teardown_server
    cat $server_output_logfile | awk '{ print "\t", $0 }'
    if (( nc_exit_status == 0 )); then
        echo "test_connection: [OK]";
    else
        echo "test_connection: [ERROR]";
	echo "${BASH_SOURCE[0]}:$test_line:0"
    fi
    echo ""
}

test_request() {
    local assert_fun=$(echo "$1")
    local config=$(echo "$2")
    local request=$(echo "$3")

    echo 'test_request: ' "$config" "$request"

    setup_server "$config"
    request_output=$(printf -- "$request" | nc -v -w 6 'localhost' "$port")
    client_exit_status=$?
    teardown_server
    server_output_logfile=$(echo "$logfile")

    local has_failed='false'
    if (( client_exit_status == 0 )) && $assert_fun; then
        echo "test_request: [OK]";
    else
        echo "test_request: [ERROR]";
	echo "client_exit_status: $client_exit_status"
	has_failed='true'
    fi

    if [[ "$has_failed" == 'true' ]] || [[ "$verbose" ==  'true' ]]; then
	echo "${BASH_SOURCE[0]}:$test_line:0"
	echo 'request output:'
        echo "$request_output" | awk '{ print "\t", $0 }'
        echo 'server_output:'
        cat "$server_output_logfile" | awk '{ print "\t", $0 }'
    fi
    echo ""
}

assert_cgi_cwd() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    local cgi_working_directory=$(basename $(echo "$request_output" | tail -1))

    if [[ "$cgi_working_directory" != "$expected_working_directory" ]]; then
        echo "expected cgi_working_directory == $expected_working_directory" | cat -e
        echo "actual   cgi_working_directory == $cgi_working_directory" | cat -e
        return 1
    fi

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}

assert_status() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}

assert_method() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    local cgi_method=$(echo "$request_output" | tail -1)

    if [[ "$cgi_method" != "$expected_cgi_method" ]]; then
        echo "expected cgi_method == $expected_cgi_method" | cat -e
        echo "actual   cgi_method == $cgi_method" | cat -e
        return 1
    fi

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}

assert_query() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    local cgi_query=$(basename $(echo "$request_output" | tail -1))

    if [[ "$cgi_query" != "$expected_cgi_query" ]]; then
        echo "expected cgi_query == $expected_cgi_query" | cat -e
        echo "actual   cgi_query == $cgi_query" | cat -e
        return 1
    fi

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}

assert_error_pages() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    local error_page=$(echo "$request_output" | tail -1)

    if [[ "$error_page" != "$expected_error_page" ]]; then
        echo "expected error_page == $expected_error_page" | cat -e
        echo "actual   error_page == $error_page" | cat -e
        return 1
    fi

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}

assert_index_page() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    local index_page=$(echo "$request_output" | tail -1)

    if [[ "$index_page" != "$expected_index_page" ]]; then
        echo "expected index_page == $expected_index_page" | cat -e
        echo "actual   index_page == $index_page" | cat -e
        return 1
    fi

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}

assert_list_directory() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    local to_find="Index of $directory"

    if !(echo "$request_output" | grep "$to_find" > /dev/null); then
        echo "expected response to contain <$to_find>"
        return 1
    fi

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}

assert_redirect() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    local expected_location_header="Location: $redirect_location"

    if !(echo "$request_output" | grep "$expected_location_header" > /dev/null); then
        echo "expected response to contain <$expected_location_header>"
        return 1
    fi

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}

assert_upload() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    if !(ls -A "$folder_to_check" | grep "$expected_filename" > /dev/null); then
        echo "expected $folder_to_check to contain $expected_filename"
	echo "ls -A $folder_to_check: $(ls -A $folder_to_check)"
        return 1
    fi

    local file_content=$(cat "$folder_to_check/$expected_filename")
    if [[ "$file_content" !=  "$expected_file_content" ]]; then
        echo "expected file with content: $(echo $expected_file_content | cat -e)"
	echo "actual file content: $(echo $file_content | cat -e)"
        return 1
    fi

    if [[ -z "$expected_other_filename" ]]; then
	return 0;
    fi

    if !(ls -A "$folder_to_check" | grep "$expected_other_filename" > /dev/null); then
        echo "expected $folder_to_check to contain $expected_other_filename"
	echo "ls -A $folder_to_check: $(ls -A $folder_to_check)"
        return 1
    fi

    local other_file_content=$(cat "$folder_to_check/$expected_other_filename")
    if [[ "$other_file_content" !=  "$expected_other_file_content" ]]; then
        echo "expected file with content: $(echo $expected_other_file_content | cat -e)"
	echo "actual file content: $(echo $other_file_content | cat -e)"
        return 1
    fi

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}

assert_allowed() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    local allow_header=$(echo "$request_output" | grep 'Allow:')
    local allow_header_lines=$(echo "$allow_header" | wc -l)
    if (( $allow_header_lines != 1 )); then
	echo "expected exactly one header with key 'Allow'"
	echo "found $allow_header_lines:"
	echo "$allow_header" | awk '{ print "\t- ", $0 }'
	return 1
    fi

    for expected_method in "${expected_allowed[@]}"; do
	if !(echo "$allow_header" | grep -E "\b$expected_method\b" > /dev/null); then
	    echo "expected Allow header to contain method $expected_method"
	    echo "found $allow_header"
	    return 1
	fi
    done

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}

assert_delete() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    if (ls -A "$folder_to_check" | grep "$expected_filename" > /dev/null); then
        echo "expected $folder_to_check to not contain $expected_filename"
		echo "ls -A $folder_to_check: $(ls -A $folder_to_check)"
        return 1
    fi

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}

assert_not_delete() {
    local status_line=$(echo "$request_output" | head -1)

    if [[ "$status_line" != "$expected_status_line" ]]; then
        echo "expected status_line == $expected_status_line" | cat -e
        echo "actual   status_line == $status_line" | cat -e
        return 1
    fi

    if !(ls -A "$folder_to_check" | grep "$expected_filename" > /dev/null); then
        echo "expected $folder_to_check to contain $expected_filename"
		echo "ls -A $folder_to_check: $(ls -A $folder_to_check)"
        return 1
    fi

    return 0
    # return keyword is for "status code" of the function execution with 0 -> ok
    # this is like the return of c main function
}





verbose='false'
### START TESTS

test_line=$(( $LINENO + 1 ))
test_connection 'config_one' 'localhost'

# when using config_one request to /cwd.cgi should have cwd=www
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
expected_working_directory='www'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_cgi_cwd \
	'config_one' \
	'GET /cwd.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when using config_one request to /42/cwd.cgi should have cwd=42
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
expected_working_directory='42'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_cgi_cwd \
	'config_one' \
	'GET /42/cwd.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when using config_one request to domain.com/cwd.cgi should have cwd=www2
# and virtual server domain is on /etc/hosts
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
expected_working_directory='www2'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_cgi_cwd \
	'config_one' \
	'GET /cwd.cgi HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n\r\n'
#

# when cgi crashes then should respond 500
expected_status_line=$(printf "HTTP/1.1 500 Internal Server Error\r")
test_line=$(( $LINENO + 1 ))
test_request \
	assert_status \
	'config_one' \
	'GET /42/error.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when cgi timesout then should respond 504
expected_status_line=$(printf "HTTP/1.1 504 Gateway Timeout\r")
test_line=$(( $LINENO + 1 ))
test_request \
	assert_status \
	'config_one' \
	'GET /42/sleep.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when on cgi request execve fails then should respond 500
expected_status_line=$(printf "HTTP/1.1 500 Internal Server Error\r")
test_line=$(( $LINENO + 1 ))
test_request \
	assert_status \
	'config_invalid_cgi_bin' \
	'GET /cwd.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when request method GET on cgiRoute cgi should receive GET as REQUEST_METHOD
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
expected_cgi_method="GET"
test_line=$(( $LINENO + 1 ))
test_request \
	assert_method \
	'config_one' \
	'GET /method.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when request method POST on cgiRoute cgi should receive POST as REQUEST_METHOD
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
expected_cgi_method="POST"
test_line=$(( $LINENO + 1 ))
test_request \
	assert_method \
	'config_one' \
	'POST /method.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when GET request with query-string on cgiRoute
# cgi should receive query-string as QUERY_STRING
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
expected_cgi_query='hello=there&abc=def'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_query \
	'config_one' \
	'GET /query.cgi?hello=there&abc=def HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when POST request with query-string on cgiRoute
# cgi should receive query-string as QUERY_STRING
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
expected_cgi_query='hello=there&abc=def'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_query \
	'config_one' \
	'POST /query.cgi?hello=there&abc=def HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when last declared cgi route matches and file not found
# should respond route error page
expected_status_line=$(printf "HTTP/1.1 404 Not Found\r")
expected_error_page='route /42/*.cgi 404'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /42/not_existing_page.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when last declared cgi route matches and times out
# should respond route error page
expected_status_line=$(printf "HTTP/1.1 504 Gateway Timeout\r")
expected_error_page='route /42/*.cgi 504'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /42/sleep.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when last declared cgi route matches and crashes
# should respond route error page
expected_status_line=$(printf "HTTP/1.1 500 Internal Server Error\r")
expected_error_page='route /42/*.cgi 500'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /42/error.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when first declared cgi route matches and file not found
# should respond route error page
expected_status_line=$(printf "HTTP/1.1 404 Not Found\r")
expected_error_page='route /**.cgi 404'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /not_existing_page.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when first declares cgi route matches and crashes
# should respond route error page
expected_status_line=$(printf "HTTP/1.1 500 Internal Server Error\r")
expected_error_page='route /**.cgi 500'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /error.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when last cgi route matches and times out
# should respond route error page
expected_status_line=$(printf "HTTP/1.1 504 Gateway Timeout\r")
expected_error_page='route /**.cgi 504'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /sleep.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when no route matches and not bad request
# should respond virtual server error page
expected_status_line=$(printf "HTTP/1.1 404 Not Found\r")
expected_error_page='virtualServer 404'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /69/not_a_page HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when bad request before match virtual server
# should respond server error page
expected_status_line=$(printf "HTTP/1.1 400 Bad Request\r")
expected_error_page='server 400'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'NOT A HTTP REQUEST'
#

# when cgi route respond status 418 and error page for 418 only on server
# should respond virtual server error page
expected_status_line=$(printf "HTTP/1.1 418 I'm a teapot\r")
expected_error_page='server 418'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /418.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when cgi route respond status 402 and error page for 402 only on virtual server
# should respond server error page
expected_status_line=$(printf "HTTP/1.1 402 Payment Required\r")
expected_error_page='virtualServer 402'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /402.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when cgi route respond status 418
# and virtual server domain is on /etc/hosts
# and error page for 418 on server and virtual server
# should respond virtual server error page
expected_status_line=$(printf "HTTP/1.1 418 I'm a teapot\r")
expected_error_page='virtualServer 418'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /418.cgi HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n\r\n'
#

# when cgi route respond status 402
# and virtual server domain is on /etc/hosts
# and error page for 402 on virtual server and route
# should respond route error page
expected_status_line=$(printf "HTTP/1.1 402 Payment Required\r")
expected_error_page='route /42/*.cgi 402'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /42/402.cgi HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n\r\n'
#

# when cgi route respond status 418
# and error page for 418 on server and route
# should respond route error page
expected_status_line=$(printf "HTTP/1.1 418 I'm a teapot\r")
expected_error_page='route /42/*.cgi 418'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_error_pages \
	'config_error_pages' \
	'GET /42/418.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when cgi route has indexFile index.cgi
# and with pathSpec /**.cgi
# and docroot/42/index.cgi exists
# and request for /42/
# then should return 200 with /42/index.cgi on body
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
expected_index_page='/42/index.cgi'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_index_page \
	'config_directory' \
	'GET /42/ HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'

# when virtual server has indexFile index.html
# and static route with pathSpec /77/*
# and docroot/77/index.html exists
# and request for /77/
# then should return 200 with /77/index.html on body
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
expected_index_page='/77/index.html'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_index_page \
	'config_directory' \
	'GET /77/ HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when virtual server has indexFile index.html
# and static route with pathSpec /**
# and docroot/69/ does not exists
# and request for /69/
# then should return 404 with route /** 404 on body
expected_status_line=$(printf "HTTP/1.1 404 Not Found\r")
expected_index_page='route /** 404'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_index_page \
	'config_directory' \
	'GET /69/ HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when static route has list directory
# and static route with pathSpec /**
# and docroot/index.html exists
# and request for /
# then should return 200 with directory listed
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
directory='/'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_list_directory \
	'config_directory' \
	'GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when static route has indexFile index.html
# and virtual server domain is on /etc/hosts
# and static route with pathSpec /**
# and docroot/42/index.html does not exists
# and request for /42/
# then should return 404 with route /** 404 on body
expected_status_line=$(printf "HTTP/1.1 404 Not Found\r")
expected_index_page='route /** 404'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_index_page \
	'config_directory' \
	'GET /42/ HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n\r\n'
#

# when static route has indexFile index.html
# and virtual server domain is on /etc/hosts
# and static route with pathSpec /**
# and docroot/77/index.html exists
# and request for /77/
# then should return 200 with /77/index.html on body
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
expected_index_page='/77/index.html'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_index_page \
	'config_directory' \
	'GET /77/ HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n\r\n'
#

# when static route has indexFile index.html
# and virtual server domain is on /etc/hosts
# and static route with pathSpec /**
# and docroot/69/ does not exists
# and request for /69/
# then should return 404 with route /** 404 on body
expected_status_line=$(printf "HTTP/1.1 404 Not Found\r")
expected_index_page='route /** 404'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_index_page \
	'config_directory' \
	'GET /69/ HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n\r\n'
#

# when static route has indexFile index.html
# and virtual server domain is on /etc/hosts
# and static route with pathSpec /**
# and docroot/index.html exists
# and request for /
# then should return 200 with /index.html on body
expected_status_line=$(printf "HTTP/1.1 200 Ok\r")
expected_index_page='/index.html'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_index_page \
	'config_directory' \
	'GET / HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n\r\n'
#

# when virtual server has 308 redirection to another hostname
# and virtual server domain is on /etc/hosts
# and has route with pathSpec /**
# then should return 308
expected_status_line=$(printf "HTTP/1.1 308 Permanent Redirect\r")
redirect_location="localhost:8080/"
test_line=$(( $LINENO + 1 ))
test_request \
	assert_redirect \
	'config_redirection' \
	'GET / HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n\r\n'
#

# when route has 307 redirection to another route
# and has route with pathSpec /55/**
# and docroot/55/ does not exist
# then should return 307
expected_status_line=$(printf "HTTP/1.1 307 Temporary Redirect\r")
redirect_location="/42/"
test_line=$(( $LINENO + 1 ))
test_request \
	assert_redirect \
	'config_redirection' \
	'GET /55/ HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when POST multipart file with filename
# and static route with spec /42/** has uploadFolder
# then expect file created with content from request after parsing multipart
# and expect response 201
upload_folder1=$(mktemp -d)
upload_folder2=$(mktemp -d)
folder_to_check="$upload_folder1"
filename="filename.txt"
file_content='some file content'
body='--====-=-=\r
Content-Disposition: form-data; name="file"; filename="'"$filename"'"\r
Content-Type: text/plain\r
\r
'"$file_content"'
--====-=-=--\r\n'
body_len=$(printf -- "$body" | wc -c)
expected_status_line=$(printf "HTTP/1.1 201 Created\r")
expected_filename="filename.txt"
expected_file_content='some file content'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_upload \
	"config_upload $upload_folder1 $upload_folder2" \
	'POST /42/ HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n'\
'Content-Type: multipart/form-data; boundary=====-=-=\r\n'\
"Content-length: $body_len\r\n\r\n$body"
#

# when POST multipart file with filename
# and virtual server domain is on /etc/hosts
# and virtual server has uploadFolder
# and static route with spec /**
# then expect file created with content from request after parsing multipart
# and expect response 201
upload_folder1=$(mktemp -d)
upload_folder2=$(mktemp -d)
folder_to_check="$upload_folder2"
filename="other_name.txt"
file_content='a different file content'
body='--====-=-=\r
Content-Disposition: form-data; name="file"; filename="'"$filename"'"\r
Content-Type: text/plain\r
\r
'"$file_content"'
--====-=-=--\r\n'
body_len=$(printf -- "$body" | wc -c)
expected_status_line=$(printf "HTTP/1.1 201 Created\r")
expected_filename="other_name.txt"
expected_file_content='a different file content'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_upload \
	"config_upload $upload_folder1 $upload_folder2" \
	'POST /42/ HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n'\
'Content-Type: multipart/form-data; boundary=====-=-=\r\n'\
"Content-length: $body_len\r\n\r\n$body"


# when POST multipart with 2 files filename.txt and othername.txt
# and virtual server domain is on /etc/hosts
# and virtual server has uploadFolder
# and static route with spec /**
# then expect file created with content from request after parsing multipart
# and expect response 201
upload_folder1=$(mktemp -d)
upload_folder2=$(mktemp -d)
folder_to_check="$upload_folder2"
filename="filename.txt"
other_filename='othername.txt'
file_content='some file content'
other_file_content='another content on file'
body='--====-=-=\r
Content-Disposition: form-data; name="file"; filename="'"$filename"'"\r
Content-Type: text/plain\r
\r
'"$file_content"'
--====-=-=\r
Content-Disposition: form-data; name="file"; filename="'"$other_filename"'"\r
Content-Type: text/plain\r
\r
'"$other_file_content"'
--====-=-=--\r\n'
body_len=$(printf -- "$body" | wc -c)
expected_status_line=$(printf "HTTP/1.1 201 Created\r")
expected_filename="filename.txt"
expected_file_content='some file content'
expected_other_filename="othername.txt"
expected_other_file_content='another content on file'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_upload \
	"config_upload $upload_folder1 $upload_folder2" \
	'POST /42/ HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n'\
'Content-Type: multipart/form-data; boundary=====-=-=\r\n'\
"Content-length: $body_len\r\n\r\n$body"
unset expected_other_filename
#

# when Server has max body size 8
# and Route has max body size 2
# and request has body with size 4
# then should return 413
expected_status_line=$(printf "HTTP/1.1 413 Request Entity Too Large\r")
test_line=$(( $LINENO + 1 ))
test_request \
	assert_status \
	'config_maxsize' \
	'POST /body_too_long HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n'\
'Content-Length: 4\r\n\r\nabcd'
#

# when Server has max body size 8
# and Route has max body size 2
# and request has body with size 1
# then should return 404
expected_status_line=$(printf "HTTP/1.1 404 Not Found\r")
test_line=$(( $LINENO + 1 ))
test_request \
	assert_status \
	'config_maxsize' \
	'POST /body_ok HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n'\
'Content-Length: 1\r\n\r\na'
#

# when Server has max body size 8
# and VirtualServer has max body size 10
# and request has body with size 12
# then should return 413
expected_status_line=$(printf "HTTP/1.1 413 Request Entity Too Large\r")
test_line=$(( $LINENO + 1 ))
test_request \
	assert_status \
	'config_maxsize' \
	'POST /body_too_long HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n'\
'Content-Length: 12\r\n\r\nabcdefghijkl'
#

# when Server has max body size 8
# and VirtualServer has max body size 10
# and request has body with size 9
# then should return 404
expected_status_line=$(printf "HTTP/1.1 404 Not Found\r")
test_line=$(( $LINENO + 1 ))
test_request \
	assert_status \
	'config_maxsize' \
	'POST /body_ok HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n'\
'Content-Length: 9\r\n\r\nabcdefghi'
#

# when request matches route
# route allows DELETE GET POST
# and request method is not allowed
# then expect 405 and allow header
expected_status_line=$(printf "HTTP/1.1 405 Method Not Allowed\r")
expected_allowed=("DELETE" "GET" "POST")
test_line=$(( $LINENO + 1 ))
test_request \
	assert_allowed \
	'config_one' \
	'PUT /index.html HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#


# when request matches route
# route allows GET POST
# and request method is not allowed
# then expect 405 and allow header
expected_status_line=$(printf "HTTP/1.1 405 Method Not Allowed\r")
expected_allowed=("GET" "POST")
test_line=$(( $LINENO + 1 ))
test_request \
	assert_allowed \
	'config_one' \
	'PATCH /42/index.cgi HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when request to static route /noperm.txt
# and file has no read permission
# then expect 403
expected_status_line=$(printf "HTTP/1.1 403 Forbidden\r")
test_line=$(( $LINENO + 1 ))
test_request \
    assert_status \
    'config_one' \
    'GET /noperm.txt HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n'
#

# when POST multipart file with filename
# and static route with spec /noAccessDir/** has uploadFolder without permission
# then expect 403
upload_folder1=./test/www/noAccessDir
upload_folder2=./test/www/noAccessDir
folder_to_check="$upload_folder2"
filename="newfile.txt"
file_content='blabla'
body='--====-=-=\r
Content-Disposition: form-data; name="file"; filename="'"$filename"'"\r
Content-Type: text/plain\r
\r
'"$file_content"'
--====-=-=--\r\n'
body_len=$(printf -- "$body" | wc -c)
expected_status_line=$(printf "HTTP/1.1 403 Forbidden\r")
expected_filename="filename.txt"
expected_file_content='some file content'
test_line=$(( $LINENO + 1 ))
test_request \
	assert_status \
	"config_upload $upload_folder1 $upload_folder2" \
	'POST /noAccessDir/acf HTTP/1.1\r\nHost: domain.com\r\nConnection: close\r\n'\
'Content-Type: multipart/form-data; boundary=====-=-=\r\n'\
"Content-length: $body_len\r\n\r\n$body"
#



# when DELETE request to existing file
# then should delete file and respond 204 No Content
delete_folder=$(mktemp -d)
folder_to_check="$delete_folder"
filename="filename.txt"
file_content='some file content'
echo "$file_content" > "$delete_folder/$filename"
expected_status_line=$(printf "HTTP/1.1 204 No Content\r")
expected_filename="filename.txt"
test_line=$(( $LINENO + 1 ))
test_request \
	assert_delete \
	"config_delete $delete_folder" \
	"DELETE /$filename HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"
unset expected_other_filename
#

# when DELETE request to existing directory
# and there are files inside
# then should not delete directory and respond 409 Conflict
delete_folder=$(mktemp -d)
folder_to_check="$delete_folder"
filename="dirname"
mkdir -p "$delete_folder/$filename"
echo "$file_content" > "$delete_folder/$filename/text.txt"
ls -R "$delete_folder/"
expected_status_line=$(printf "HTTP/1.1 409 Conflict\r")
expected_filename="dirname"
test_line=$(( $LINENO + 1 ))
test_request \
	assert_not_delete \
	"config_delete $delete_folder" \
	"DELETE /$filename/ HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"
unset expected_other_filename
#

# when DELETE request to existing directory
# and there are no files inside
# then should delete directory and respond 204 No Content
delete_folder=$(mktemp -d)
folder_to_check="$delete_folder"
filename="$delete_folder"
mkdir -p "$delete_folder/$filename"
ls -R "$delete_folder/"
expected_status_line=$(printf "HTTP/1.1 204 No Content\r")
expected_filename="dirname"
test_line=$(( $LINENO + 1 ))
test_request \
	assert_delete \
	"config_delete $delete_folder" \
	"DELETE /$filename/ HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"
unset expected_other_filename
#
