#! /bin/sh

if [ $# -lt 1 ]; then 
    echo "Usage: hole.sh path"
    exit 1
fi

path="$1"
space=0
size=0
for line in $(find "${path}" -type f 2>/dev/null); do
    # echo "${line}"
    space=$(du -k "${line}" | awk '{print $1}')
    space_bytes=$((${space}*1024))
    #size=$(ls -l "${line}" | awk '{print $5}')
    size=$(stat -c "%s" "${line}")
    # echo "${line}: ${size}->${space}"
    if [ ${space_bytes} -lt ${size} ]; then 
        echo "file ${line} has hole, space ${space_bytes} < size ${size}"
    fi
done
