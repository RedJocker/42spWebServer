#!/bin/bash

routes_file=$(find . -name 'siege-tested-routes.txt')

if [ -n "routes_file" ]; then
    siege -v --file "$routes_file"
else
    echo "could not find siege-tested-routes.txt"
    echo "try running command on root folder of sp42_webserver"
fi
