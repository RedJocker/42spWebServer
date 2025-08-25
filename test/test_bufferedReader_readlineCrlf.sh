#!/bin/bash

actual_basedir=$(pwd)
expected_basedir='/42spWebServer'
expected_basedir_regex="$expected_basedir$"
test_cdriver='./test_buff_readlineCrlf'

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
	make BUFFER_SIZE="$1" re "$test_cdriver"  1> /dev/null 2> /dev/null
	build_status="$?"

	if (( build_status != 0 )); then
		exit $build_status;
	fi
}

run_test_command() {
	echo "Testing with BUFFER_SIZE=$BUFFER_SIZE"
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
echo -n hello > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

BUFFER_SIZE=1
setup_with_buffer_size $BUFFER_SIZE

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
reading
reading
reading
eof: hello"""
expected_status="0"
run_test_command

printf 'hello\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
reading
reading
reading
reading
done: hello"""
expected_status="0"
run_test_command


printf 'hello\r\nnot read line\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
reading
reading
reading
reading
done: hello"""
expected_status="0"
run_test_command

printf 'hello\r\nanother line read\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=2
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
reading
reading
reading
reading
done: hello
reading
reading
reading
reading
reading
reading
reading
reading
reading
reading
reading
reading
reading
reading
reading
reading
reading
reading
reading
done: another line read"""
expected_status="0"
run_test_command


BUFFER_SIZE=2
setup_with_buffer_size $BUFFER_SIZE

echo -n hello > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
reading
eof: hello"""
expected_status="0"
run_test_command

printf 'hello\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
reading
done: hello"""
expected_status="0"
run_test_command

printf 'hello\r\nnot read line\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
reading
done: hello"""
expected_status="0"
run_test_command

printf 'hello\r\nanother line read\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=2
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
reading
done: hello
reading
reading
reading
reading
reading
reading
reading
reading
reading
done: another line read"""
expected_status="0"
run_test_command

BUFFER_SIZE=3
setup_with_buffer_size $BUFFER_SIZE

echo -n hello > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
eof: hello"""
expected_status="0"
run_test_command

printf 'hello\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
done: hello"""
expected_status="0"
run_test_command

printf 'hello\r\n
not read line\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
done: hello"""
expected_status="0"
run_test_command

printf 'hello\r\nanother line read\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=2
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
done: hello
reading
reading
reading
reading
reading
reading
done: another line read"""
expected_status="0"
run_test_command

BUFFER_SIZE=4
setup_with_buffer_size $BUFFER_SIZE

echo -n hello > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
reading
eof: hello"""
expected_status="0"
run_test_command

printf 'hello\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
done: hello"""
expected_status="0"
run_test_command

printf 'hello\r\nnot read line\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
done: hello"""
expected_status="0"
run_test_command

printf 'hello\r\nanother line read\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=2
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
reading
done: hello
reading
reading
reading
reading
reading
done: another line read"""
expected_status="0"
run_test_command

BUFFER_SIZE=16
setup_with_buffer_size $BUFFER_SIZE

echo -n hello > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead") 
expected="""reading
reading
eof: hello"""
expected_status="0"
run_test_command

printf 'hello\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE"  "$linesToRead")
expected="""reading
done: hello"""
expected_status="0"
run_test_command

printf 'hello\r\nnot read line\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=1
command=("$test_cdriver" "$TEMP_FILE"  "$linesToRead")
expected="""reading
done: hello"""
expected_status="0"
run_test_command

printf 'hello\r\nanother line read\r\n' > $TEMP_FILE
printf "TEMP_FILE:<%s>" "$(cat -e "$TEMP_FILE")"
echo ""

linesToRead=2
command=("$test_cdriver" "$TEMP_FILE" "$linesToRead")
expected="""reading
done: hello
reading
done: another line read"""
expected_status="0"
run_test_command
