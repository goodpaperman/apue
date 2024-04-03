#!/bin/sh
groupadd test
echo "create group ok"

useradd -g test foo
useradd -g test bar
foo_uid=$(id -u foo)
bar_uid=$(id -u bar)
root_uid=0
echo "create user ok"
echo "    foo: ${foo_uid}"
echo "    bar: ${bar_uid}"
echo "    root: ${root_uid}"

cd /tmp
chown root:test ./setuid
echo "test foo"
su foo -c ./setuid

chmod u+s ./setuid
echo "test set-uid root"
su foo -c ./setuid

echo "test setreuid(-1, foo)"
su foo -c "./setuid -1 ${foo_uid}"

echo "test setreuid(-1, bar)"
su foo -c "./setuid -1 ${bar_uid}"

echo "test setreuid(foo, foo)"
su foo -c "./setuid ${foo_uid} ${foo_uid}"

echo "test setreuid(root, foo)"
su foo -c "./setuid ${root_uid} ${foo_uid}"


userdel foo
userdel bar
echo "remove user ok"

rm -rf /home/foo
rm -rf /home/bar
echo "remove user home ok"

groupdel test
echo "delete group ok"
