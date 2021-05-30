#! /bin/sh
# set -x

echo "switch to user lippman"
# ensure new user can create file
cd /tmp

groups lippman 
echo "show user and their group ok"

touch this_is_a_demo_file
ls -lhrt this_is_a*

rm this_is_a*
echo "remove testing file ok"

# change group back
sudo usermod -g lippman lippman
cd -
echo "test file group unchange over"

