#! /bin/bash
groupadd men
groupadd share
echo "create group ok"

useradd lippman -G share,men
useradd -g share steven
useradd -g men caveman 
useradd -g men paperman
echo "create user ok"

groups lippman steven caveman paperman
echo "show user and their group ok"

# too see user's supgrp store where -- in /etc/group
#cat /etc/passwd
#echo ""
#cat /etc/group

# TODO: add testing script here
if false; then 
    # case: file group unchanged
    cp ./file_group_unchanged_1.sh /tmp/
    cp ./file_group_unchanged_2.sh /tmp/
    su - lippman -s /tmp/file_group_unchanged_1.sh
    # change current owner's group
    usermod -g share lippman
    su - lippman -s /tmp/file_group_unchanged_2.sh
    usermod -g lippman lippman
fi

if false; then 
    # case: setuid setgid order
    cp ./setugid /tmp/
    cp ./setuid_setgid_order_1.sh /tmp/
    cp ./setuid_setgid_order_2.sh /tmp/
    cp ./setuid_setgid_order_3.sh /tmp/
    su - lippman -s /tmp/setuid_setgid_order_1.sh
    su - caveman -s /tmp/setuid_setgid_order_2.sh
    su - lippman -s /tmp/setuid_setgid_order_3.sh
fi

if false; then 
    # case: process groups unchanged
    cp ./setugid /tmp/
    cp ./process_supgid_unchanged.sh /tmp/
    rm /tmp/should_wait
    su - lippman -s /tmp/process_supgid_unchanged.sh
    # change current owner's supplementary group
    usermod -G lippman lippman
    touch /tmp/should_wait
    su - lippman -s /tmp/process_supgid_unchanged.sh
fi

if false; then 
    # case: permission group fusing
    cp ./probe_file_perm.sh /tmp/
    cp ./perm_group_fuse_1.sh /tmp/
    cp ./perm_group_fuse_2.sh /tmp/
    rm /tmp/this_is_a_test_file 2>/dev/null
    su - steven -s /tmp/perm_group_fuse_1.sh
    rm /tmp/this_is_a_demo_file 2>/dev/null
    su - caveman -s /tmp/perm_group_fuse_1.sh
    # start access test
    su - steven -s /tmp/perm_group_fuse_2.sh
    su - caveman -s /tmp/perm_group_fuse_2.sh
    su - paperman -s /tmp/perm_group_fuse_2.sh
    su - lippman -s /tmp/perm_group_fuse_2.sh
fi

if false; then 
    # case: share with svtx
    cp ./probe_file_perm.sh /tmp/
    cp ./share_with_svtx_1.sh /tmp/
    cp ./share_with_svtx_2.sh /tmp/
    su - steven -s /tmp/share_with_svtx_1.sh
    su - caveman -s /tmp/share_with_svtx_1.sh
    su - lippman -s /tmp/share_with_svtx_1.sh
    su - paperman -s /tmp/share_with_svtx_1.sh
    # start access test
    su - paperman -s /tmp/share_with_svtx_2.sh
    su - lippman -s /tmp/share_with_svtx_2.sh
    su - caveman -s /tmp/share_with_svtx_2.sh
    su - steven -s /tmp/share_with_svtx_2.sh
    rm -rf /tmp/share
fi

if false; then 
    # case: setgid parent dir
    cp ./setgid_parent_dir.sh /tmp/
    su - lippman -s /tmp/setgid_parent_dir.sh
    su - caveman -s /tmp/setgid_parent_dir.sh
    su - paperman -s /tmp/setgid_parent_dir.sh
    su - steven -s /tmp/setgid_parent_dir.sh
    #ls -lh "/tmp/share/"
    rm -rf /tmp/share
fi

if false; then
    # case: change ogid clear setgid
    rm /tmp/this_is_a_demo_file 2>/dev/null
    rm -rf /tmp/this_is_a_demo_dir 2>/dev/null
    cp ./chgrp_clear_setgid.sh /tmp/
    su - lippman -s /tmp/chgrp_clear_setgid.sh
    chmod ug+s,o+t "/tmp/this_is_a_demo_file"
    chown caveman "/tmp/this_is_a_demo_file"
    ls -lh "/tmp/this_is_a_demo_file"
    chmod ug+s,o+t "/tmp/this_is_a_demo_dir"
    chown steven "/tmp/this_is_a_demo_dir"
    ls -lhd "/tmp/this_is_a_demo_dir"
fi

if false; then 
    # case: write clear setuid 
    cp ./setugid /tmp/
    cp ./write_clear_setuid.sh /tmp/
    su - lippman -s /tmp/write_clear_setuid.sh
fi

userdel -r lippman
userdel -r steven
userdel -r caveman
userdel -r paperman
echo "delete user ok"

groupdel men
groupdel share
echo "delete group ok"
