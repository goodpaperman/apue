#! /bin/sh
rm file.lck
truncate -s 100 file.lck
./filelock file.lck LOCKWAIT READ 0 SET 100 &
sleep 1
./filelock file.lck TESTLOCK WRITE 50 SET 1 &
sleep 1
./filelock file.lck UNLOCK READ 50 SET 1 &
sleep 1
./filelock file.lck TESTLOCK WRITE 50 SET 1 &
sleep 1
./filelock file.lck TESTLOCK WRITE 25 SET 1 &
sleep 1
./filelock file.lck TESTLOCK WRITE 75 SET 1 &
sleep 1
