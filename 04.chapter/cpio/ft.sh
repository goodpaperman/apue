#! /bin/sh
#ls | cpio -oav > z.c.cpio
cpio -iv < z0.cpio
#cpio -iv < z.cpio
#cpio -ivm < z0.cpio
#cpio -ivm < z.cpio
tar xvf acc.c.tar
#tar xvfm acc.c.tar
#tar xvf acc.c.tar --atime-preserve
