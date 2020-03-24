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
  ./dbh db55 insert $key $data
done <demo5

IFS="$OLD_IFS"
     
