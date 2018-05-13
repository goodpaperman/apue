#! /bin/sh
#ps -ejH
#ps axjf
#ps xfo pid,ppid,pgid,sid,tpgid,stat,tty,command | awk "NR==1||/pts\/2/"
ps xfo pid,ppid,pgid,sid,tpgid,stat,tty,command 
