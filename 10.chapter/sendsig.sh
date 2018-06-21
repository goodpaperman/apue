#! /bin/sh

for ((i=0; i<10000; i++)); do 
    pkill -"$1" lostsig
done

echo "kill total 10000 times"
