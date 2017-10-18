#! /bin/sh
make 
touch tip.lck
chown yh:yh tip tip.lck
chmod u+s tip
