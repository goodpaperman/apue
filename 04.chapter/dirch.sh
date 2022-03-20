#! /bin/sh

main()
{
    if [ $# -lt 2 ]; then 
        echo "Usage dirch dirname [loop]"
        exit 1
    fi

    base="$1"
    dir="$2"
    cd "${base}"
    while true; do
        cd "${dir}"
        if [ $? -ne 0 ]; then 
            echo "cd ${dir} failed"
            exit 2
        else 
            #echo "cd to `/usr/bin/pwd`"
            echo "cd to `pwd`"
        fi
    done
}

main "$@"
