#!/bin/bash

source function.sh

list "not an exit" "promoting" >redirect.log
echo <redirect.log
echo $?
