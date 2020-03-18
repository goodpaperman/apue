#! /bin/sh
OLD_IFS="$IFS"
IFS=" "
while read line
do
#  echo $line
  array=($line)
  key=${array[0]}
  data=${array[1]}
  echo "$key -> $data"
  ./db db5 insert $key $data
done <demo2

IFS="$OLD_IFS"
     
