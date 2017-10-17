#! /bin/sh
touch a0 a1 a2 c0 d0 f3 g0
mkdir c2 e0 e1 e2 e3 f1 f2 
cd f2
mkdir demo
cd ../
ln -s a0 b0
ln -s a1 b1
ln -s a2 b2
ln g0 h0
ls -lR
mv -T a0 c0
mv -T a1 c1
mv -T a2 c2
mv -T b0 d0
mv -T b1 d1
mv -T b2 d2
mv -T e0 f0
mv -T e1 f1
mv -T e2 f2
mv -T e3 f3
mv -T g0 h0
mv -T f2/demo f2/demo/tmp
ls -lR
rm -rf a* b* c* d* e* f* g* h*
