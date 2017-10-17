#! /bin/sh
n=524288
for((i=1;i<21;i++))
{
  echo "test file.$i with buffer size $n"
  time ./copy $n < "file.$i" > /dev/null
  n=$((n/2))
}
