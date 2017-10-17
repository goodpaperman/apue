#! /bin/sh
mkdir foo
touch foo/abc
ln -s ../foo foo/testdir
ls -l foo
rm -rf foo
