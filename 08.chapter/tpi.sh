#! /bin/sh
make 
touch tpi.lck
chown root:root tpi tpi.lck
chmod u+s tpi
#ps auo "pid fname uid euid suid %cpu %mem args" | grep tpi
