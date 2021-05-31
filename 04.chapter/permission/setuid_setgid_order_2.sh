#! /bin/sh
echo "switch to user caveman"
# ensure new user can create file
cd /tmp

sh setuid_demo.sh &
sh setgid_demo.sh &
sh setuid_setgid_demo.sh &

echo "start setuid/setgid program ok"
ps -axo pid,ruid,rgid,euid,egid,suid,sgid,supgid,cmd

echo "waiting them to exit..."
wait

rm setuid_demo.sh
rm setgid_demo.sh
rm setuid_setgid_demo.sh
echo "remove testing file ok"
