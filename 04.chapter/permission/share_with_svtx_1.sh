#! /bin/sh
echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

if [ ! -d /tmp/share ]; then 
    mkdir /tmp/share
    # allow every to create file 
    chmod ugo+rwx,o+t /tmp/share
    ls -lhd /tmp/share
fi

touch "/tmp/share/$(whoami)"
# 'rwx rw- r--'
chmod 0731 "/tmp/share/$(whoami)"
ls -lh "/tmp/share/$(whoami)"

echo "prepare testing file ok"
