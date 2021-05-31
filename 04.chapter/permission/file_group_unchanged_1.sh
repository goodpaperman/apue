#! /bin/sh
# set -x

echo "switch to user lippman"
# ensure new user can create file
cd /tmp

# lippman belongs to lippman/share/men
# can change between them
touch this_is_a_test_file
ls -lh this_is_a_test_file

# change current owner's group
sudo usermod -g share lippman
groups lippman
echo "show user and their group ok"

cd -
echo "test file group unchange over"
