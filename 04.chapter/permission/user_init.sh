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
# case 01
cp ./file_group_unchanged_1.sh /tmp/
cp ./file_group_unchanged_2.sh /tmp/
su - lippman -s /tmp/file_group_unchanged_1.sh
su - lippman -s /tmp/file_group_unchanged_2.sh

# case 02
#cp ./setuid_setgid_order_1.sh /tmp/
#cp ./setuid_setgid_order_2.sh /tmp/
#su - steven -s /tmp/setuid_setgid_order_1.sh
#su - caveman -s /tmp/setuid_setgid_order_2.sh

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
