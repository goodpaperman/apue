#! /bin/bash
echo "switch to user lippman"
# ensure new user can create file
cd /tmp

# show user & group id for later use
id

./setugid &
echo "start program ok"
ps -ao pid,ruid,rgid,euid,egid,suid,sgid,supgid,cmd

if [ -f "/tmp/should_wait" ]; then 
    echo "waiting them to exit..."
    wait
fi
