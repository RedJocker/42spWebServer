#!/bin/bash

set -e

OBJ_DIR=$(mktemp -d)
BIN_DIR=$(mktemp -d)

cleanup() {
	rm -rf "$OBJ_DIR" "$BIN_DIR"
}
trap cleanup EXIT

echo "[1/2] Compiling HTTP Request test..."
c++ -Wall -Wextra -Werror -std=c++98 -I./src -c ./src/http/Request.cpp -o "$OBJ_DIR/Request.o"
c++ -Wall -Wextra -Werror -std=c++98 -I./src ./test/test_request_line.cpp "$OBJ_DIR/Request.o" -o "$BIN_DIR/test_request_line"

echo "[2/2] Running test_request_line..."
"$BIN_DIR/test_request_line" && \
	echo -e "test_request_line: [OK]" || \
	echo -e "test_request_line: [ERROR]"

