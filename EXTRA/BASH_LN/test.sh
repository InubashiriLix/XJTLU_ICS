#!/bin/bash

echo "Shell passed arguments:"
echo "executable file name: ${0}"
# echo "the first argument: $1"
echo "the first argument: ${1}"
# echo "the second argument: $2"
echo "the second argument: ${2}"
# echo "the third argument: $3"
echo "the third argument: ${3}"
echo "the total number of arguments: $#"
echo "list all the arguments with separator space: $*"
echo "the id of this scrips' id: $$"
echo "the last id of bg process: ${!}"
