#! /bin/sh 
./dgrams 1024 &
sleep 1
./dgramc 1024 &
./dgramc 1024 &
./dgramc 1024 &


