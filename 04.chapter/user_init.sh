#! /bin/bash
groupadd common
groupadd share
echo "create group ok"

useradd lippman -G share,common
useradd steven -G share
useradd caveman -G common
useradd paperman -G common
echo "create user ok"

groups lippman steven caveman paperman
echo "show user and their group ok"

groupdel common
groupdel share
echo "delete group ok"

groups lippman steven caveman paperman
echo "show user and their group ok"

userdel lippman
userdel steven
userdel caveman
userdel paperman
echo "delete user ok"

rm -rf /home/lippman
rm -rf /home/steven
rm -rf /home/caveman
rm -rf /home/paperman
echo "remve user home dir ok"

