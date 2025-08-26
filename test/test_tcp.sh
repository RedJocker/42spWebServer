#!/bin/bash

actual_basedir=$(pwd)
expected_basedir='/42spWebServer'
expected_basedir_regex="$expected_basedir$"

if ! [[ $actual_basedir =~ $expected_basedir_regex ]]; then
    echo 'execute test on base dir'
    echo "current directory $actual_basedir"
    echo "expected directory ending with $expected_basedir"
    echo 'you may change expected_basedir on test script if basedir has a different name'
    exit 42
fi

make test_tcp
build_status="$?"

if (( build_status != 0 )); then
    exit $build_status;
fi

output_file=$(mktemp)

setup_server() {
    ./test_tcp $1 > $output_file &
    SERVER_PID=$!
    sleep 0.4
}

teardown_server() {
    kill -9 $SERVER_PID 2> /dev/null
}

test_connection() {
    echo 'test_connection:'
    setup_server 'connection'
    nc -v -z -w 2 localhost 8080
    nc_exit_status=$?
    cat $output_file
    if (( nc_exit_status == 0 )); then
		echo "test_connection: [OK]";
    else
		echo "test_connection: [ERROR]";
    fi
    teardown_server
}

test_read() {
    echo "test_read: $1 $2"
    setup_server "read $1"
    printf "$2" | nc -v -w 2 localhost 8080
    nc_exit_status=$?
	teardown_server
	actual=$(cat $output_file)
	if (( nc_exit_status != 0 )); then
		echo "test_read: $1 $2 [ERROR]"
		echo "actual status: $nc_exit_status"
		echo "expected status: 0"
	elif [[ "$actual" != "$3" ]] ; then
		echo "test_read: [ERROR] $1 $2"
		echo "actual:"
		echo "$actual"
		echo ""
		echo "expected:"
		echo "$3"
		echo ""
	else
		echo "test_read: [OK] $1 $2"
    fi
}

test_readlineCrlf() {
    echo "test_readlineCrlf: $1" 
    setup_server "readlineCrlf"
    printf "$1" | nc -v -w 2 localhost 8080
    nc_exit_status=$?
    teardown_server
    actual=$(cat $output_file)
    if (( nc_exit_status != 0 )); then
	echo "test_readlineCrlf: [ERROR]"
	echo "actual status: $nc_exit_status"
	echo "expected status: 0"
    elif [[ "$actual" != "$2" ]] ; then
	echo "test_readlineCrlf: [ERROR] $1"
	echo "actual:"
	echo "$actual"
	echo ""
	echo "expected:"
	echo "$2"
	echo ""
    else
	echo "test_readlineCrlf: [OK] $1"
    fi
}

test_echo() {
    echo "test_echo: $1"
    expected_nc="$2"
    expected_server="$3"
    setup_server "echo"
    actual_nc="$(printf "$1" | nc -v -w 2 localhost 8080)"
    nc_exit_status=$?
    teardown_server
    actual_server=$(cat $output_file)
    if (( nc_exit_status != 0 )); then
	echo "test_echo: [ERROR]"
	echo "actual status: $nc_exit_status"
	echo "expected status: 0"
    elif [[ "$actual_server" != "$expected_server" ]] ; then
	echo "test_echo: [ERROR] $1"
	echo "actual server:"
	echo "$actual_server"
	echo ""
	echo "expected server:"
	echo "$expected_server"
	echo ""
    elif [[ "$actual_nc" != "$expected_nc" ]] ; then
	echo "test_echo: [ERROR] $1"
	echo "actual nc:"
	printf "$actual_nc" | hexdump -C
	echo ""
	echo "expected nc:"
	printf "$expected_nc" | hexdump -C
	echo ""
    else
	echo "test_echo: [OK] $1"
    fi
}


test_connection
test_read 5 hello 'Opening server listening on port 8080 and fd 3
done: hello
Closing server with port 8080 and fd 3'
test_read 2 hello 'Opening server listening on port 8080 and fd 3
done: he
Closing server with port 8080 and fd 3'
test_read 15 hello 'Opening server listening on port 8080 and fd 3
eof: hello
Closing server with port 8080 and fd 3'

test_readlineCrlf hello 'Opening server listening on port 8080 and fd 3
eof: hello
Closing server with port 8080 and fd 3'

test_readlineCrlf 'hello\r\n' 'Opening server listening on port 8080 and fd 3
done: hello
Closing server with port 8080 and fd 3'

test_echo $'hello\r\n'  $'hello\r' 'Opening server listening on port 8080 and fd 3
done: hello
message sent
Closing server with port 8080 and fd 3'
