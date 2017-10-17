#! /bin/sh
dd if=/dev/zero of=file.1 bs=807159 count=128
echo "generate file.1"
for ((i=2; i<21; i++))
{
  cp file.1 "file.$i"
  echo "generate file.$i"
}
