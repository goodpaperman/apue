#! /bin/sh
# set -x

echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

groups $(whoami) 
echo "show user and their group ok"

touch this_is_a_demo_file
ls -lh this_is_a*

rm this_is_a*
echo "remove testing file ok"

