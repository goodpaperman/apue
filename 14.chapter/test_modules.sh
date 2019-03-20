#! /bin/bash
#./test_stream.sh | awk -F':' '{ print $1 }' 
./test_stream.sh | awk -F':' '{ print $1 }' | xargs -n 1 ./ilist
