#! /bin/sh
#export LD_PRELOAD=./libhack.so:$LD_PRELOAD
LD_PRELOAD=./libhack.so ./login 123456 &
# can sleep in strcmp and use pstack to see stacks
# need install before use:
#   yum install pstack
pstack $!
# to see all the process stacks
#ps -eo pid --no-headers | xargs -l1 pstack
#ps -eo pid --no-headers | xargs -i pstack {}
