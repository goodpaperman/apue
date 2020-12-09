#! /bin/bash
useradd lippmann
useradd steven
useradd caveman
useradd paperman
echo "create user ok"

groupadd common
groupadd share
echo "create group ok"

usermod -a -G share lippmann
usermod -a -G share steven
usermod -a -G common lippmann
usermod -a -G common caveman
usermod -a -G common paperman
echo "add user to group ok"

groups lippmann steven caveman paperman
echo "show user and their group ok"

groupdel common
groupdel share
echo "delete group ok"

groups lippmann steven caveman paperman
echo "show user and their group ok"

userdel lippmann
userdel steven
userdel caveman
userdel paperman
echo "delete user ok"

rm -rf /home/lippmann
rm -rf /home/steven
rm -rf /home/caveman
rm -rf /home/paperman
echo "remve user home dir ok"

