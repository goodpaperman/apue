#!/bin/sh
groupadd test
echo "create group ok"

useradd -g test bar
root_uid=$(id -u root)
bar_uid=$(id -u bar)
echo "create user ok"
echo "    root: ${root_uid}"
echo "    bar: ${bar_uid}"

cd /tmp
chown bar:test ./setuid
echo "test root"
./setuid

chmod u+s ./setuid
echo "test set-uid bar"
./setuid

echo "test setuid(root)"
./setuid ${root_uid}

echo "test seteuid(root)"
./setuid ${root_uid} noop noop

echo "test setreuid(bar, root)"
./setuid ${bar_uid} ${root_uid}

echo "test setreuid(-1, root)"
./setuid -1 ${root_uid}

echo "test setreuid(bar, -1)"
./setuid ${bar_uid} -1

userdel bar
echo "remove user ok"

rm -rf /home/bar
echo "remove user home ok"

groupdel test
echo "delete group ok"
