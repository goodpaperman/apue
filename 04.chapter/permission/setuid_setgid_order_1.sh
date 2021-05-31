#! /bin/sh
echo "switch to user steven"
# ensure new user can create file
cd /tmp

# create setuid/setgid/setuid & setgid program
echo "sleep 10" > setuid_demo.sh
chmod u+s,ugo+wx !$
ls -lh !$

echo "sleep 10" > setgid_demo.sh
chmod g+s,ugo+wx !$
ls -lh !$

echo "sleep 10" > setuid_setgid_demo.sh
chmod ug+s,ugo+wx !$
ls -lh !$

echo "create testing setuid/setgid file ok"
