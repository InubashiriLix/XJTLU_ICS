#!/bin/bash

wc -l <redirect.log
wc -l re.sh redirect.log failed.py

./failed.py 2>failed.log
./failed.py 2>>failed.log
./failed.py &>all.log
./failed.py &>>all.log

./failed2.py >all2.log 2>&1
