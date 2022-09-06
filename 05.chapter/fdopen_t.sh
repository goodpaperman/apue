#! /bin/sh

oflags=("r" "w" "a" "r+" "w+" "a+")
modes=("r" "r+" "w" "w+" "a" "a+")
for oflag in ${oflags[@]}
do
    for mode in ${modes[@]}
    do
        ./fdopen_t abc.txt ${oflag} ${mode}
    done
done
