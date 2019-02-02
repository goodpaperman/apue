#! /bin/sh
rm file.lck
truncate -s 100 file.lck
./filelock file.lck LOCK READ 0 SET 50 &
./filelock file.lck LOCK READ 10 SET 60 &
./filelock file.lck LOCK READ 20 SET 70 &
./filelock file.lck LOCK READ 30 SET 80 &
./filelock file.lck LOCK READ 40 SET 90 &
./filelock file.lck LOCK READ 50 SET 100 &
./filelock file.lck LOCK READ 10 SET 70 &
./filelock file.lck LOCK READ 20 SET 80 &
./filelock file.lck LOCK READ 30 SET 90 &
./filelock file.lck LOCK READ 40 SET 100 &
#./filelock file.lck LOCK WRITE 0 SET 100 &
