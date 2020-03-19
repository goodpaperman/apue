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
  resp=`./db db5 delete $key`
  if [ $? != 0 ]; then 
#cmd will report it
#echo "delete $key failed"
    n=$(($n+1))
  fi 
done <demo5

./db db5 walk
echo "total failure: $n"
./db db5 dump

IFS="$OLD_IFS"
     
