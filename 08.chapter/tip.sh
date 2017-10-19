#! /bin/sh
make 
touch tip.lck
chown yh:yh tip tip.lck
#chown root:root tip tip.lck
chmod u+s tip
#ps auo "pid fname uid euid suid %cpu %mem args" | grep tip
