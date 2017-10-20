#! /bin/sh
make 
touch pti.lck
chown root:root pti pti.lck
chmod u+s pti
#ps auo "pid fname uid euid suid %cpu %mem args" | grep pti
