#! /bin/sh
echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

if [ ! -e this_is_a_test_file ]; then 
    touch this_is_a_test_file
    # '--- r-x -w-'
    chmod 0052 this_is_a_test_file
    ls -lh this_is_a_test_file
fi

if [ ! -e this_is_a_demo_file ]; then 
    touch this_is_a_demo_file
    # '--- r-x -w-'
    chmod 0052 this_is_a_demo_file
    ls -lh this_is_a_demo_file
fi

echo "prepare testing file ok"
