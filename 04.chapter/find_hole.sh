#! /bin/sh

function main()
{
    local path="."
    if [ $# -gt 0 ]; then 
        path="$1"
    fi

    echo "detect hole under ${path}"
    local size=0
    local space=0
    for file in $(find "${path}" -type f); do
        if [ -f "${file}" ]; then 
            size=$(stat -c "%s" "${file}")
            space=$(($(du -k "${file}" | awk '{print $1}')*1024))
            if [ ${size} -gt ${space} ]; then 
                echo "${file} has hole, space ${space}, size ${size}"
            fi
        else 
            # file-name has chinese character ?
            #echo "no ${file}"
            :
        fi
    done
    echo "done!"
}


main "$@"
