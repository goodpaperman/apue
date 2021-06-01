#! /bin/sh
if [ $# -lt 1 ]; then 
    echo "Usage: probe_file_perm.sh file_to_test"
    exit 1
fi

filename=$1
if [ -e "$filename" ]; then 
    echo "exist"
else
    echo "not exist"
    exit 1
fi

if [ -r "$filename" ]; then 
    echo "can read"
else
    echo "can NOT read"
fi

if [ -w "$filename" ]; then 
    echo "can write"
else
    echo "can NOT write"
fi

if [ -x "$filename" ]; then 
    echo "can execute"
else
    echo "can NOT exeucte"
fi

exit 0

