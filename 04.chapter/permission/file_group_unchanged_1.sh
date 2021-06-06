#! /bin/sh
# set -x

echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

# lippman belongs to lippman/share/men
# can change between them
touch this_is_a_test_file
ls -lh this_is_a_test_file
