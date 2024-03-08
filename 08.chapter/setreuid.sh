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
#chown bar:test ./setuid
echo "test foo"
./setuid

#chmod u+s ./setuid
#echo "test set-uid bar"
#su foo -c ./setuid

echo "test setreuid(bar, foo)"
./setuid ${bar_uid} ${foo_uid}

echo "test setreuid(foo, bar)"
./setuid ${foo_uid} ${bar_uid}

echo "test setreuid(-1, foo)"
./setuid -1 ${foo_uid}

echo "test setreuid(bar, -1)"
./setuid ${bar_uid} -1

echo "test setreuid(bar, bar)"
./setuid ${bar_uid} ${bar_uid}

echo "test setreuid(foo, foo)"
./setuid ${foo_uid} ${foo_uid}

userdel bar
userdel foo
echo "remove user ok"

rm -rf /home/bar
rm -rf /home/foo
echo "remove user home ok"

groupdel test
echo "delete group ok"
