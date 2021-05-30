#! /bin/bash
groupadd men
groupadd share
echo "create group ok"

useradd lippman -G share,men
useradd -g share steven
useradd -g men caveman
useradd -g men paperman
echo "create user ok"

groups lippman steven caveman paperman
echo "show user and their group ok"

# TODO: add testing script here
cp ./file_group_unchanged.sh /tmp/
su - lippman -s /tmp/file_group_unchanged.sh

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

groupdel men
groupdel share
echo "delete group ok"
