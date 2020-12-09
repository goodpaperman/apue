#! /bin/bash
groupadd common
groupadd share
echo "create group ok"

useradd lippmann -G share,common
useradd -g share steven
useradd -g common caveman
useradd -g common paperman
echo "create user ok"

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

groupdel common
groupdel share
echo "delete group ok"

