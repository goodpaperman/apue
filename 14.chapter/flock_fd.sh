#! /bin/sh
rm file.lck
truncate -s 100 file.lck
./filelockfd file.lck LOCKWAIT WRITE 0 SET 30 CLOSE &
./filelockfd file.lck LOCKWAIT WRITE 30 SET 30 DUP &
./filelockfd file.lck LOCKWAIT WRITE 60 SET 30 REOPEN &
sleep 1
./filelock file.lck LOCKWAIT WRITE 10 SET 20  &
./filelock file.lck LOCKWAIT WRITE 30 SET 20  &
./filelock file.lck LOCKWAIT WRITE 60 SET 20  &
