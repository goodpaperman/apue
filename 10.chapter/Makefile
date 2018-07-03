all: coredump testsig demosig execsig lostsig interwait errnomt getpwnam_r localtime_r

localtime_r: localtime_r.o apue.o
	gcc -Wall -g $^ -o $@

localtime_r.o: localtime_r.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

getpwnam_r: getpwnam_r.o apue.o
	gcc -Wall -g $^ -o $@

getpwnam_r.o: getpwnam_r.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

errnomt: errnomt.o apue.o
	gcc -Wall -g $^ -o $@ -pthread

errnomt.o: errnomt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

interwait: interwait.o apue.o
	gcc -Wall -g $^ -o $@

interwait.o: interwait.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

lostsig: lostsig.o apue.o
	gcc -Wall -g $^ -o $@

lostsig.o: lostsig.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

execsig: execsig.o apue.o
	gcc -Wall -g $^ -o $@

execsig.o: execsig.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

demosig: demosig.o apue.o
	gcc -Wall -g $^ -o $@

demosig.o: demosig.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

testsig: testsig.o apue.o
	gcc -Wall -g $^ -o $@

testsig.o: testsig.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

coredump: coredump.o apue.o
	gcc -Wall -g $^ -o $@

coredump.o: coredump.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp coredump testsig demosig execsig lostsig interwait errnomt getpwnam_r localtime_r
	@echo "end clean"

.PHONY: clean
