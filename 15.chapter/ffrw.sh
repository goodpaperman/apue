#! /bin/sh 
#./ffread hello.ff &
#sleep 1
#./ffwrite hello.ff

./ffwrite hello.ff &
#sleep 1
sleep 11
./ffread hello.ff

