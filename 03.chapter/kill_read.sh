#! /bin/sh

#while true; do
#  pkill -INT read_inter
#done

pid=`pidof read_inter`
echo pidof read_inter = $pid
for ((i=0; i<1024; ++i))
do 
  echo kill $pid $i
  #pkill -INT read_inter
  kill -INT $pid
  sleep 0.001s
done

#pkill -KILL read_inter
echo kill $pid with SIGKILL
kill -KILL $pid
