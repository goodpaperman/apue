#! /bin/sh 
if [ ! -p "hello.ff" ]; then 
    mkfifo hello.ff
    echo "mkfifo for hello.ff"
fi

## will return ENXIO for write process
#./ffwrite hello.ff
#sleep 1
#./ffread hello.ff &

./ffread hello.ff &
sleep 1
./ffwrite hello.ff

#./ffwrite hello.ff &
##sleep 1
#sleep 11
#./ffread hello.ff

