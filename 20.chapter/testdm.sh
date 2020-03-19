#! /bin/sh
OLD_IFS="$IFS"
IFS=" "
while read line
do
#  echo $line
  array=($line)
  key=${array[0]}
  data=${array[1]}
  ./db db5 delete $key &
done <demo5

sleep 3
./db db5 walk
./db db5 dump

IFS="$OLD_IFS"
     
