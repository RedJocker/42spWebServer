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

setup_server() {
    ./test_tcp &
    SERVER_PID=$!
    sleep 0.1
}

teardown_server() {
    kill -9 $SERVER_PID 2> /dev/null 
}

test_connection() {
    echo 'test_connection:' 
    setup_server
    nc -v -z localhost 8080
    nc_exit_status=$?
    if (( nc_exit_status == 0 )); then
		echo "test_connection: [OK]";
    else
		echo "test_connection: [ERROR]";
    fi
    teardown_server
}


test_connection





