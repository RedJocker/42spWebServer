#!/bin/bash

actual_basedir=$(pwd)
expected_basedir='/42spWebServer'
expected_basedir_regex="$expected_basedir$"
test_cdriver='./test_buff_read'

if ! [[ $actual_basedir =~ $expected_basedir_regex ]]; then
    echo 'execute test on base dir'
    echo "current directory $actual_basedir"
    echo "expected directory ending with $expected_basedir"
    echo 'you may change expected_basedir on test script if basedir has a different name'
    exit 42
fi

make "$test_cdriver"
build_status="$?"

if (( build_status != 0 )); then
	exit $build_status;
fi


setup_with_buffer_size() {
	echo "Building with BUFFER_SIZE=$1"
	make BUFFER_SIZE="$1" re "$test_cdriver" 1> /dev/null 2> /dev/null
	build_status="$?"

	if (( build_status != 0 )); then
		exit $build_status;
	fi
}

run_test_command() {
	echo "Testing with BUFFER_SIZE=$BUFFER_SIZE and toRead=$toRead"
	actual=$("${command[@]}" 2>&1)
    status="$?"
    if [[ "$actual" != "$expected" ]]; then
		echo "[BAD] test: $command ";
		echo "actual:"
		echo "$actual";
		echo ""
		echo "expected:"
		echo "$expected"
		echo ""
		echo "input file:"
		cat "$TEMP_FILE"
		exit 42;
    elif (( status != expected_status )); then
		"[BAD] test: $command ";
		echo "actual status: $status"
		echo "expected status: $expected_status"
		exit $status
    else
		echo "[GOOD] test: $command ";
    fi
}

TEMP_FILE=$(mktemp)
echo hello > $TEMP_FILE

BUFFER_SIZE=1
setup_with_buffer_size $BUFFER_SIZE

toRead=1
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
h"""
expected_status="0"
run_test_command

toRead=2
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
reading
he"""
expected_status="0"
run_test_command

toRead=3
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
reading
reading
hel"""
expected_status="0"
run_test_command

toRead=4
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
reading
reading
reading
hell"""
expected_status="0"
run_test_command

toRead=5
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
reading
reading
reading
reading
hello"""
expected_status="0"
run_test_command

BUFFER_SIZE=2
setup_with_buffer_size $BUFFER_SIZE

toRead=1
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
h"""
expected_status="0"
run_test_command

toRead=2
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
he"""
expected_status="0"
run_test_command

toRead=3
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
reading
hel"""
expected_status="0"
run_test_command

toRead=4
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
reading
hell"""
expected_status="0"
run_test_command

toRead=5
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
reading
reading
hello"""
expected_status="0"
run_test_command

BUFFER_SIZE=16
setup_with_buffer_size $BUFFER_SIZE

toRead=1
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
h"""
expected_status="0"
run_test_command

toRead=2
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
he"""
expected_status="0"
run_test_command

toRead=3
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
hel"""
expected_status="0"
run_test_command

toRead=4
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
hell"""
expected_status="0"
run_test_command

toRead=5
command=("$test_cdriver" "$TEMP_FILE" "$toRead")
expected="""reading
hello"""
expected_status="0"
run_test_command

