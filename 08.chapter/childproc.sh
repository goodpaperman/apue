#! /bin/sh
./childtime "dd if=/dev/urandom of=./out1 bs=1M count=512" "dd if=/dev/urandom of=./out2 bs=1M count=1024"

