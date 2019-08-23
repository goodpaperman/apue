#! /bin/sh
for ((i=0; i<5; i++))
{
    ./xsisem 127 1 &
}

./xsisem 127 1 put 0 5
#for ((i=0; i<5; i++))
#{
#    ./xsisem 127 1 put &
#}
