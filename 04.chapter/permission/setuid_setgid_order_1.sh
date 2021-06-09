#! /bin/sh
echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

# show user & group id for later use
id

# create setuid/setgid/setuid & setgid program
cp setugid setuid_demo 
chmod u+s,ugo+wx /tmp/setuid_demo
ls -lh setuid_demo

cp setugid setgid_demo
chmod g+s,ugo+wx setgid_demo
ls -lh setgid_demo

cp setugid setuid_setgid_demo
chmod ug+s,ugo+wx setuid_setgid_demo
ls -lh setuid_setgid_demo

echo "create testing setuid/setgid file ok"
