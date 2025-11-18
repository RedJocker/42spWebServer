#!/bin/bash

actual_basedir=$(basename $(pwd))
expected_basedir='42spWebServer'
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
    sleep 0.4
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
    request_output=$(printf "$request" | nc -v -w 6 'localhost' "$port")
    client_exit_status=$?
    teardown_server
    server_output_logfile=$(echo "$logfile")


    if (( client_exit_status == 0 )) && $assert_fun; then
        echo "test_request: [OK]";
    else
        echo 'request output:'
        echo "$request_output" | awk '{ print "\t", $0 }'
        echo 'server_output:'
        cat "$server_output_logfile" | awk '{ print "\t", $0 }'
        echo "test_request: [ERROR]";
	echo "${BASH_SOURCE[0]}:$test_line:0"
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
# TODO use config for timeout time to make faster timeout
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
