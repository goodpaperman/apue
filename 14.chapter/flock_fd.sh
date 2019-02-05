#! /bin/sh
rm file.lck
truncate -s 100 file.lck
./filelockfd file.lck LOCKWAIT WRITE 0 SET 50 &
sleep 1
./filelock file.lck LOCKWAIT WRITE 10 SET 60 &
