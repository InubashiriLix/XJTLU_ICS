#!/bin/bash

val=$(expr 1 + 1)
echo $val

val2=$((1 + 1))
echo $val2

echo $((1 + 1))
echo $((1 - 1))
echo $((1 * 114514))
echo $(expr 1 \* 114514)
shit1=1
shit2=1
shit3=3
if [[ $shit1 == "$shit2" ]]; then
    echo "1 is qual to 1"
fi
if [[ $shit1 -eq "$shit2" ]]; then
    echo "1 is qual to 1"
fi
if [[ $shit1 -eq "$shit2" || $shit2 -ne "$shit3" ]]; then
    echo "fuck"
fi

if [ $shit1 -eq "$shit2" -o $shit2 -ne "$shit3" ]; then
    echo "fuck"
fi

if [ $shit1 -eq "$shit2" ] || [ $shit2 -ne "$shit3" ]; then
    echo "fuck"
fi

a="this is a piece of shit"
b="this is a piece of shit"
c="this is not a piece of shit"
if [[ $a = "$b" ]]; then
    echo "\"${a}\" and \"${b}\" are the same"
fi

if [[ $a = "$c" ]]; then
    echo "\"${a}\" and \"${c}\" are the same"
else
    echo "\"${a}\" and \"${c}\" are the different"
fi

if [[ $a != "$c" ]]; then
    echo "\"${a}\" and \"${c}\" are the diff"
fi

empty=""
if [[ -z $empty ]]; then
    echo "the variable called empty is empty"
fi

if [[ $empty ]]; then
    echo "the variable called empty is not empty"
else
    echo "the variable called empty is empty"
fi
