#!/bin/bash

shit="this is waht you want???"
Shit="this is a piece of shit"
shit_word="you can never get over it"

echo "${shit}"
echo "${Shit}"
echo ${shit_word}

for skill in "Take shit" "write shit code" "map the end"; do
    echo "I am good at ${skill}"
done

shit="no more running"
echo "${shit}"
readonly shit
# shit="this is the end" # this will cause the error
unset Shit
echo "${Shit}" # no output

declare -i nshit=1
echo "${nshit}"
nshit+=1
echo "${nshit}"

myarray=(1 2 3 4 5)
for num in "${myarray[@]}"; do
    echo "the number: ${num}"
done

declare -A dict
dict["name"]="inubashiri"
dict["age"]=22
echo "${dict["name"]}"
echo "${dict["age"]}"

echo "$PATH"

str='this is a string'
echo "${str}"
str1='this is str1'
str2='this is str2'
# about the concat the strings
echo "hello, "$str1""
echo "hello, ${str1}"
echo 'hello, '$str1'!'
echo 'hello, ${str1}!' # you should use double quotes for this case

# get the length of the string
echo "${#str}"
echo $((${#str1} + ${#str2}))

# the array part
arr_name=(1 2 3 4 5)
for num in "${arr_name[@]}"; do
    echo "${num}"
done

declare -A dict_shit
dict_shit["shit"]="shit"
echo "${dict_shit["shit"]}"

dict_num[1]=2
dict_num[2]=3
for num in "${dict_num[@]}"; do
    echo "${num}"
done
echo "${dict_num[1]}"

# the cooments
: <<'COMMENT'
这是注释的部分。
可以有多行内容。
COMMENT
