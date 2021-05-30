#! /bin/sh
# set -x

echo "switch to user lippman"
# ensure new user can create file
cd /tmp

# lippman belongs to lippman/share/men
# can change between them
touch this_is_a_test_file
ls -lhrt this_is_a_test_file

# change current owner's group
# using full path to avoid usermod not find..
sudo usermod -g share lippman
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

