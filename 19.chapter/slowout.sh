#! /bin/sh
for ((i=0; i<10; i=i+1)) {
    echo "turn $i"
    ping www.glodon.com -c 4
#sleep 4
    resp=$(read -p "type any char to continue")
}
