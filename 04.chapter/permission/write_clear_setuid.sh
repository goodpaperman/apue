#! /bin/sh
echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

# create setuid & setgid program
cp setugid setugid_demo
chmod ug+s,ugo+wx /tmp/setugid_demo
ls -lh setugid_demo

echo "create testing setuid/setgid file ok"
echo "1" >> setugid_demo

echo "write 1 bytes into executable file"
ls -lh setugid_demo

chmod ug+s,ugo+wx /tmp/setugid_demo
truncate -s 8K setugid_demo

echo "truncate executable file to 8K"
ls -lh setugid_demo

rm setugid_demo
echo "remove testing file ok"
