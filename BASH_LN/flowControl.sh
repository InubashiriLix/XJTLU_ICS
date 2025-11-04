#!/bin/bash
if [[ 1 == 1 ]] || [[ 1 != 2 ]]; then echo "shit"; fi
if test 1 == 1; then echo "shit"; fi
a=1
b=2
if test ${a} -lt ${b}; then echo "a is less than b"; fi

for num in 1 2 3 4 5; do
    echo ${num}
done

declare -i int
int=1
while test ${int} -lt 5; do
    let "int++"
done
echo ${int}
unset int

let a=5+5
let b=a-1
echo $b

c=$((1 + 14))
echo $c

declare -i int
int=5
while [[ int -gt 0 ]]; do
    int=$((int - 1))
    echo $int
done

# while read FILM; do
#     echo "$FILM is a piece of shit"
# done

int=10
until [ $int -lt 5 ]; do
    int=$(($int - 1))
    echo $int
    if [ $int -eq 7 ]; then break; fi
done

read NUM
case $NUM in
1)
    echo "shit1"
    ;;
2)
    echo "shit2"
    ;;
3)
    echo "shit3"
    ;;
*)
    echo "esc"
    ;;
esac
