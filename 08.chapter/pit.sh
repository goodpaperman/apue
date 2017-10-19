#! /bin/sh
make 
touch pit.lck
chown root:root pit pit.lck
chmod u+s pit
#ps auo "pid fname uid euid suid %cpu %mem args" | grep pit
