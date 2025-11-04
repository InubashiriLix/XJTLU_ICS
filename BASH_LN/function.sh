#!/bin/bash

kontrol() {
    echo "function start"
    echo "function end"
}

readAndSum() {
    declare -i int1
    declare -i int2
    echo "enter int1"
    read int1
    echo "enter int2"
    read int2

    return $(($int1 + $int2))
}

kontrol

# readAndSum
# echo "sum: ${?}" # the return value use $?

list() {
    shit=""
    for arg in "$@"; do
        echo "Argument: ${arg}"
    done
}

sum() {
    declare -i sum
    sum=0
    for arg in "$@"; do
        sum=$((sum + arg))
    done

    return $sum
}

shit2=10
list "shit" "$shit2"

sum 1 2 3 4 5
echo $?
