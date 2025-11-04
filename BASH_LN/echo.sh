#!/bin/bash

echo -e "\033[31mRed Text\033[0m"
echo -e "\033[42;31mGreen Background with Red Text\033[0m"
echo -e "\033[31mRed Text"
echo -e "shit\033[0m"

echo -n "Progress: ["
for i in {1..20}; do
    echo -n "#"
    sleep 0.1
done
echo "] Done!"

echo -e "\n\033[1mSystem Menu\033[0m"
echo "1. Check disk space"
echo "2. List running processes"
echo "3. Show system info"
echo -n "Please enter your choice [1-3]: "

echo "the start of shit" >shit.log        # override the log file
echo "this is a piece of shit" >>shit.log # append the log file
