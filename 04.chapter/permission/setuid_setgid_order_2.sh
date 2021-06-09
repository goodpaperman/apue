#! /bin/sh
echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

# show user & group id for later use
id

./setuid_demo &
./setgid_demo &
./setuid_setgid_demo &

echo "start setuid/setgid program ok"
ps -ao pid,ruid,rgid,euid,egid,suid,sgid,supgid,cmd

echo "waiting them to exit..."
wait

