#! /bin/sh
echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

if [ ! -d /tmp/share ]; then 
    mkdir /tmp/share
    # allow everyone to create file 
    chmod ugo+rwx /tmp/share
    chmod g+s /tmp/share
    ls -lhd /tmp/share
fi

mkdir "/tmp/share/$(whoami)_dir"
touch "/tmp/share/$(whoami)_file"
#ls -lh "/tmp/share/$(whoami)_*"

echo "prepare testing file ok"
