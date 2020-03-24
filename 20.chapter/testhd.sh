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
  resp=`./dbh db55 delete $key`
  if [ $? != 0 ]; then 
#cmd will report it
#echo "delete $key failed"
    n=$(($n+1))
  fi 
done <demo5

./dbh db55 walk
echo "total failure: $n"
./dbh db55 dump

IFS="$OLD_IFS"
     
