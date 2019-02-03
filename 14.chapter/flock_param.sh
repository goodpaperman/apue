#! /bin/sh
rm file.lck
truncate -s 100 file.lck
./filelock file.lck LOCK WRITE 0 SET 50 &
./filelock file.lck LOCK WRITE 0 END -50 &
#./filelock file.lck LOCK WRITE 0 CUR -50 &
#./filelock file.lck LOCK WRITE 0 SET 0 &
./filelock file.lck LOCK WRITE 0 END 0 &
sleep 1
truncate -s 150 file.lck
echo "extend file with length 50"
./filelock file.lck LOCK WRITE 0 END -50 &
