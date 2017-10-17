all:  stat_fd acc umask_fd chmod_fd hold follow_links ftw ftimes dtimes z0 trunc0 dirproc dirmode dirftw dirscan dirch devid dirmk devfd

stat_fd: stat_fd.o apue.o
	gcc -Wall -g $^ -o $@

stat_fd.o: stat_fd.c ../apue.h 
	gcc -Wall -g -c $< -o $@

acc: acc.o apue.o
	gcc -Wall -g $^ -o $@

acc.o: acc.c ../apue.h 
	gcc -Wall -g -c $< -o $@

umask_fd: umask_fd.o apue.o
	gcc -Wall -g $^ -o $@

umask_fd.o: umask_fd.c ../apue.h 
	gcc -Wall -g -c $< -o $@

chmod_fd: chmod_fd.o apue.o
	gcc -Wall -g $^ -o $@

chmod_fd.o: chmod_fd.c ../apue.h 
	gcc -Wall -g -c $< -o $@

hold: hold.o apue.o
	gcc -Wall -g $^ -o $@

hold.o: hold.c ../apue.h 
	gcc -Wall -g -c $< -o $@

follow_links: follow_links.o apue.o
	gcc -Wall -g $^ -o $@ #-DGNU_SOURCE

follow_links.o: follow_links.c ../apue.h 
	gcc -Wall -g -c $< -o $@

ftw: ftw.o apue.o
	gcc -Wall -g $^ -o $@ #-DGNU_SOURCE

ftw.o: ftw.c ../apue.h 
	gcc -Wall -g -c $< -o $@

ftimes: ftimes.o apue.o
	gcc -Wall -g $^ -o $@ 

ftimes.o: ftimes.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D_ATFILE_SOURCE

dtimes: dtimes.o apue.o
	gcc -Wall -g $^ -o $@

dtimes.o: ftimes.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -DSEE_BASE -D_ATFILE_SOURCE

z0: z0.o apue.o
	gcc -Wall -g $^ -o $@

z0.o: z.c ../apue.h
	gcc -Wall -g -c $< -o $@

trunc0: trunc0.o apue.o
	gcc -Wall -g $^ -o $@ 

trunc0.o: trunc0.c ../apue.h 
	gcc -Wall -g -c $< -o $@

dirproc: dirproc.o apue.o
	gcc -Wall -g $^ -o $@ 

dirproc.o: dirproc.c ../apue.h 
	gcc -Wall -g -c $< -o $@

dirmode: dirmode.o apue.o
	gcc -Wall -g $^ -o $@ 

dirmode.o: dirmode.c ../apue.h 
	gcc -Wall -g -c $< -o $@

dirftw: dirftw.o apue.o
	gcc -Wall -g $^ -o $@ 

dirftw.o: dirftw.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -DUSE_RELATIVE_PATH

dirscan: dirscan.o apue.o
	gcc -Wall -g $^ -o $@ 

dirscan.o: dirscan.c ../apue.h 
	gcc -Wall -g -c $< -o $@

dirch: dirch.o apue.o
	gcc -Wall -g $^ -o $@ 

dirch.o: dirch.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D_GNU_SOURCE

dirmk: dirmk.o apue.o
	gcc -Wall -g $^ -o $@ 

dirmk.o: dirmk.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -std=c99 -D_GNU_SOURCE

devfd: devfd.o apue.o
	gcc -Wall -g $^ -o $@ 

devfd.o: devfd.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D_GNU_SOURCE

devid: devid.o apue.o
	gcc -Wall -g $^ -o $@ 

devid.o: devid.c ../apue.h 
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp stat_fd acc umask_fd chmod_fd hold follow_links ftw ftimes z0 trunc0 dirproc dirmode dirftw dirscan dirch devid dirmk devfd
	@echo "end clean"

.PHONY: clean
