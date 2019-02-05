#! /bin/sh
rm file.lck
truncate -s 100 file.lck 
./flockdeadpair file.lck 0 &
./flockdeadpair file.lck 1 &
