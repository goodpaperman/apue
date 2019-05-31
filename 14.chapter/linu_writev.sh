#! /bin/sh

for ((n=1; n<11; n++))
{
    time ./linu_writev tmp$(($n*2)) $((1024*$n)) 1
    time ./linu_writev tmp$(($n*2+1)) $((1024*$n)) 0
}
