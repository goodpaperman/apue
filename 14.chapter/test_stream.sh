#! /bin/bash
# this script used to test a file under /dev, which is a stream
#find /dev/* -prune | sed '/tty[pqr]./d' 
#find /dev/* -prune | sed '/tty[pqr]./d' | xargs ./icanput 
#find /dev/* -prune | sed '/tty[pqr]./d' | xargs -n 1 ./icanput 
find /dev/* -prune | sed '/tty[pqr]./d' | xargs -n 1 ./icanput | grep 'stream device'
