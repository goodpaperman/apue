#! /bin/bash
#
FILE=/etc/shadow

awk -F ':' '{
 if ($2 != "*" && $2 != "!!" ) {
 NAME=$1;
 MAX_CH=$3+$5;
 EXPIRE=$3+$8;
 CMD="echo -e "NAME" \"\t\`date -d \"1970-01-01 "MAX_CH" days\" +%Y/%m\` \t`date -d \"1970-01-01 "EXPIRE" days\" +%Y/%m\`\" "
 system(CMD);
 }
}' $FILE 2>/dev/null |sort -k 3 | awk '{printf("%-15s%-15s%-15s\n",$1,$2,$3)}'
