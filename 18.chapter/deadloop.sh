#! /bin/sh
var=1
while :
do
  echo this is $var
  var=$(($var+1))
  usleep 100000
done
