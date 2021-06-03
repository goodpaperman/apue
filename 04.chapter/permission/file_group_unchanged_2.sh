#! /bin/sh
# set -x

echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

# lippman belongs to lippman/share/men
groups $(whoami) 
echo "show user and their group ok"

touch this_is_a_demo_file
ls -lh this_is_a*

rm this_is_a*
echo "remove testing file ok"

# change group back
sudo usermod -g lippman $(whoami)
cd -
echo "test file group unchange over"
