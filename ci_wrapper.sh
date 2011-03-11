#!/bin/bash

make test

if [ $? -ne 0 ]; then
    growlnotify -a Xcode -n simple-ci -d bravo5.simpleci -t CI -m "BUILD FAILED" 
# else
#     growlnotify -a Xcode -n simple-ci -d bravo5.simpleci -m "Success" 
fi
