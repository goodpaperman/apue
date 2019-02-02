#! /bin/sh
rm file.lck
truncate -s 100 file.lck
./filelock file.lck LOCK READ 0 SET 9 &
./filelock file.lck LOCK READ 10 SET 9 &
./filelock file.lck LOCK WRITE 20 SET 9 &
./filelock file.lck LOCK READ 30 SET 9 &
./filelock file.lck LOCK READ 40 SET 9 &
./filelock file.lck LOCK READ 50 SET 9 &
./filelock file.lck LOCK WRITE 60 SET 9 &
./filelock file.lck LOCK WRITE 70 SET 9 &
./filelock file.lck LOCK WRITE 80 SET 9 &
./filelock file.lck LOCK READ 90 SET 9 &
#./filelock file.lck LOCK READ 0 SET 100 &
