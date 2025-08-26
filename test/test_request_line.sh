#!/usr/bin/env bash

if [ -z "$BASH_VERSION" ]; then exec bash "$0" "$@"; fi

set -e

OBJ_DIR="$(mktemp -d)"
BIN_DIR="$(mktemp -d)"

cleanup() {
    rm -rf "$OBJ_DIR" "$BIN_DIR"
}
trap cleanup EXIT

echo "[1/2] Compiling HTTP Request test..."
c++ -Wall -Wextra -Werror -std=c++98 -I./src -c ./src/http/Request.cpp -o "$OBJ_DIR/Request.o"
c++ -Wall -Wextra -Werror -std=c++98 -I./src ./test/test_request_line.cpp "$OBJ_DIR/Request.o" -o "$BIN_DIR/test_request_line"

echo "[2/2] Running test_request_line..."
if "$BIN_DIR/test_request_line"; then
    echo -e "test_request_line: [OK] ✅"
else
    echo -e "test_request_line: [ERROR] ❌"
    exit 1
fi
