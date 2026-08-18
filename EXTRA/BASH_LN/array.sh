#!/bin/bash

declare -A site
site["google"]="www.shit.com"
site["shit"]="www.baidu.com"
site["shutup"]="www.csdn.com"

echo "the keys of this array: ${!site[*]}"
# echo "the keys of this array: ${!site[@]}"
echo "the length of this array: ${#site[*]}"
