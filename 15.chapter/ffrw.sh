#! /bin/sh 
if [ ! -p "hello.ff" ]; then 
    mkfifo hello.ff
    echo "mkfifo for hello.ff"
fi

## test ENXIO for write process
#./ffwrite hello.ff
#sleep 1
#./ffread hello.ff &

# test read first
#./ffread hello.ff &
#sleep 1
#./ffwrite hello.ff

# test multi-read/write
./ffread hello.ff &
./ffread hello.ff &
sleep 1
./ffwrite hello.ff &
./ffwrite hello.ff

# test write first
#./ffwrite hello.ff &
##sleep 1
#sleep 11
#./ffread hello.ff

