#! /bin/sh
len=1
#len=50
rm file.lck
truncate -s 100 file.lck
./flocksplit file.lck 100 &
sleep 1
./filelock file.lck TESTLOCK WRITE 0 SET $len &
./filelock file.lck TESTLOCK WRITE 90 SET $len &
./filelock file.lck TESTLOCK WRITE 50 SET $len &
sleep 2
./filelock file.lck TESTLOCK WRITE 0 SET $len &
./filelock file.lck TESTLOCK WRITE 90 SET $len &
./filelock file.lck TESTLOCK WRITE 50 SET $len &
sleep 2
./filelock file.lck TESTLOCK WRITE 0 SET $len &
./filelock file.lck TESTLOCK WRITE 90 SET $len &
./filelock file.lck TESTLOCK WRITE 50 SET $len &
