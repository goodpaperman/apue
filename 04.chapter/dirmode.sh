#! /bin/sh 
mkdir testdir
touch testdir/test
echo test rwx
./dirmode testdir test
echo test -wx
chmod u-r testdir
./dirmode testdir test
echo test r-x
chmod u+r-w testdir
./dirmode testdir test
echo test rw-
chmod u+w-x testdir
./dirmode testdir test
echo test r--
chmod u-w testdir
./dirmode testdir test
echo test -w-
chmod u-r+w testdir
./dirmode testdir test
echo test --x
chmod u-w+x testdir
./dirmode testdir test
echo test over
chmod u+rwx testdir
rm -rf testdir
