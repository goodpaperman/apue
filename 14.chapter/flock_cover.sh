#! /bin/sh
len=1
rm file.lck
truncate -s 100 file.lck
./flockcover file.lck 100 &
sleep 1
./filelock file.lck TESTLOCK WRITE 12 SET $len &
./filelock file.lck TESTLOCK WRITE 37 SET $len &
./filelock file.lck TESTLOCK WRITE 50 SET $len &
./filelock file.lck TESTLOCK WRITE 62 SET $len &
./filelock file.lck TESTLOCK WRITE 87 SET $len &
sleep 2
./filelock file.lck TESTLOCK WRITE 12 SET $len &
./filelock file.lck TESTLOCK WRITE 37 SET $len &
./filelock file.lck TESTLOCK WRITE 50 SET $len &
./filelock file.lck TESTLOCK WRITE 62 SET $len &
./filelock file.lck TESTLOCK WRITE 87 SET $len &
sleep 2
./filelock file.lck TESTLOCK WRITE 12 SET $len &
./filelock file.lck TESTLOCK WRITE 37 SET $len &
./filelock file.lck TESTLOCK WRITE 50 SET $len &
./filelock file.lck TESTLOCK WRITE 62 SET $len &
./filelock file.lck TESTLOCK WRITE 87 SET $len &
