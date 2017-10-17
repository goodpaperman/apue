#! /bin/sh
sed -e 's/\\/\\\\/g' -e 's/"/\\"/g' -e 's/%/%%/g' -e 's/\(.*\)/printf("\1\\n")/' conf.awk.c > conf.awk
