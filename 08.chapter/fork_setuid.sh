#!/bin/sh
groupadd test
echo "create group ok"

useradd -g test foo
useradd -g test bar
foo_uid=$(id -u foo)
bar_uid=$(id -u bar)
echo "create user ok"
echo "    foo: ${foo_uid}"
echo "    bar: ${bar_uid}"

cd /tmp
# chown foo:test ./setuid
chown bar:test ./fork_setuid
chmod u+s ./fork_setuid
su foo -c ./fork_setuid

userdel bar
userdel foo
echo "remove user ok"

rm -rf /home/bar
rm -rf /home/foo
echo "remove user home ok"

groupdel test
echo "delete group ok"
