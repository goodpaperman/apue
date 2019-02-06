#! /bin/sh
rm file.lck
truncate -s 100 file.lck
./flockend file.lck &
sleep 1
./filelock file.lck TESTLOCK WRITE 0 SET 1 &
./filelock file.lck TESTLOCK WRITE 0 SET 0 &
./filelock file.lck TESTLOCK WRITE 99 SET 1 &
./filelock file.lck TESTLOCK WRITE 100 SET 1 &
./filelock file.lck TESTLOCK WRITE 0 END 1 &
./filelock file.lck TESTLOCK WRITE 0 END 0 &
sleep 1
#truncate -s 101 file.lck
#echo "incrase file size by 1 byte"
sleep 2
./filelock file.lck TESTLOCK WRITE 0 SET 1 &
./filelock file.lck TESTLOCK WRITE 0 SET 0 &
./filelock file.lck TESTLOCK WRITE 99 SET 1 &
./filelock file.lck TESTLOCK WRITE 100 SET 1 &
./filelock file.lck TESTLOCK WRITE 0 END 1 &
./filelock file.lck TESTLOCK WRITE 0 END 0 &
