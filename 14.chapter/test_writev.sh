#! /bin/sh

for ((n=0; n<10; n++))
{
    time ./linu_writev tmp$n $((104857*$n))
}
