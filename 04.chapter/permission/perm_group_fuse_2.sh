#! /bin/sh
echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

echo "checking this_is_a_test_file"
./probe_file_perm.sh "this_is_a_test_file"
echo ""

echo "checking this_is_a_demo_file"
./probe_file_perm.sh "this_is_a_demo_file"
echo ""
