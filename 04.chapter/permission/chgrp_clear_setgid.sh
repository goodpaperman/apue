#! /bin/sh
echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

touch "/tmp/this_is_a_demo_file"
chmod ug+xs "/tmp/this_is_a_demo_file"
chmod o+xt "/tmp/this_is_a_demo_file"
ls -lh "/tmp/this_is_a_demo_file"

mkdir "/tmp/this_is_a_demo_dir"
chmod ug+xs "/tmp/this_is_a_demo_dir"
chmod o+t "/tmp/this_is_a_demo_dir"
ls -lhd "/tmp/this_is_a_demo_dir"

#chown lippman:men "/tmp/this_is_a_demo_file"
#chown :men "/tmp/this_is_a_demo_file"
chgrp men "/tmp/this_is_a_demo_file"
#chgrp share "/tmp/this_is_a_demo_file"
#chgrp lippman "/tmp/this_is_a_demo_file"
ls -lh "/tmp/this_is_a_demo_file"

chgrp share "/tmp/this_is_a_demo_dir"
ls -lhd "/tmp/this_is_a_demo_dir"

echo "chgrpp clear setgid over"
