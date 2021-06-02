#! /bin/sh
echo "switch to user $(whoami)"
# ensure new user can create file
cd /tmp

# $1: file to move
try_move_file ()
{
    local file="$1"
    local file_new="$1.new"
    mv "$file" "$file_new" 2>/dev/null
    if [ $? -eq 0 ]; then 
        echo "can move"
        # move back
        mv "$file_new" "$file"
    else
        echo "can NOT move"
    fi
}

echo "checking /tmp/share/steven"
./probe_file_perm.sh "/tmp/share/steven"
try_move_file "/tmp/share/steven"
echo ""

echo "checking /tmp/share/caveman"
./probe_file_perm.sh "/tmp/share/caveman"
try_move_file "/tmp/share/caveman"
echo ""

echo "checking /tmp/share/lippman"
./probe_file_perm.sh "/tmp/share/lippman"
try_move_file "/tmp/share/lippman"
echo ""

echo "checking /tmp/share/paperman"
./probe_file_perm.sh "/tmp/share/paperman"
try_move_file "/tmp/share/paperman"
echo ""
