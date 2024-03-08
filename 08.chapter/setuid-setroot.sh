#!/bin/sh
groupadd test
echo "create group ok"

useradd -g test foo
foo_uid=$(id -u foo)
root_uid=0
echo "create user ok"
echo "    foo: ${foo_uid}"
echo "    root: ${root_uid}"

cd /tmp
chown root:test ./setuid
echo "test foo"
su foo -c ./setuid

chmod u+s ./setuid
echo "test set-uid root"
su foo -c ./setuid

echo "test setuid(foo)"
su foo -c "./setuid ${foo_uid}"

echo "test seteuid(foo)"
su foo -c "./setuid ${foo_uid} noop noop"

echo "test setreuid(root, foo)"
su foo -c "./setuid ${root_uid} ${foo_uid}"

echo "test setreuid(-1, foo)"
su foo -c "./setuid -1 ${foo_uid}"

echo "test setreuid(root, -1)"
su foo -c "./setuid ${root_uid} -1"

echo "test setreuid(foo, bar)"
su foo -c "./setuid ${foo_uid} ${bar_uid}"

echo "test setreuid(bar, foo)"
su foo -c "./setuid ${bar_uid} ${foo_uid}"


userdel foo
echo "remove user ok"

rm -rf /home/foo
echo "remove user home ok"

groupdel test
echo "delete group ok"
