#! /bin/sh
OLD_IFS="$IFS"
IFS=" "
n=0
while read line
do
#  echo $line
  array=($line)
  key=${array[0]}
  data=${array[1]}
  resp=`./db db5 query $key $data`
  if [ "$resp" != "$data" ]; then 
    echo "query $key with $resp != $data"
    n=$(($n+1))
  fi 
done <demo5
echo "total different: $n"

IFS="$OLD_IFS"
     
