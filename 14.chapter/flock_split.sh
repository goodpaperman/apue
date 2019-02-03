#! /bin/sh
rm file.lck
truncate -s 100 file.lck
./flocksplit file.lck 100 &
sleep 1
./filelock file.lck TESTLOCK WRITE 0 SET 1 &
./filelock file.lck TESTLOCK WRITE 90 SET 1 &
./filelock file.lck TESTLOCK WRITE 50 SET 1 &
sleep 2
./filelock file.lck TESTLOCK WRITE 0 SET 1 &
./filelock file.lck TESTLOCK WRITE 90 SET 1 &
./filelock file.lck TESTLOCK WRITE 50 SET 1 &
sleep 2
./filelock file.lck TESTLOCK WRITE 0 SET 1 &
./filelock file.lck TESTLOCK WRITE 90 SET 1 &
./filelock file.lck TESTLOCK WRITE 50 SET 1 &
