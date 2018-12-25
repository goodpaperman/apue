all: tattrsize tconcurr mattrshare mattrtype tattrtimeout rwattrshare cattrshare flock getenv static key cancel signal

signal: signal.o apue.o
	gcc -Wall -g $^ -o $@ -pthread

signal.o: signal.c ../apue.h
	gcc -Wall -g -c $< -o $@

cancel: cancel.o 
	gcc -Wall -g $^ -o $@ -pthread

cancel.o: cancel.c 
	gcc -Wall -g -c $< -o $@

key: key.o 
	gcc -Wall -g $^ -o $@ -pthread

key.o: key.c 
	gcc -Wall -g -c $< -o $@

static: static.o
	g++ -Wall -g $^ -o $@ -pthread

static.o: static.c
	g++ -Wall -g -c $< -o $@

getenv: getenv.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

getenv.o: getenv.c ../apue.h
	gcc -Wall -g -c $< -o $@

flock: flock.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

flock.o: flock.c ../apue.h
	gcc -Wall -g -c $< -o $@ -D_GNU_SOURCE

cattrshare: cattrshare.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

cattrshare.o: cattrshare.c ../apue.h
	gcc -Wall -g -c $< -o $@

rwattrshare: rwattrshare.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

rwattrshare.o: rwattrshare.c ../apue.h
	gcc -Wall -g -c $< -o $@

tattrtimeout: tattrtimeout.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

tattrtimeout.o: tattrtimeout.c ../apue.h
	gcc -Wall -g -c $< -o $@ -D_GNU_SOURCE

mattrtype: mattrtype.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

mattrtype.o: mattrtype.c ../apue.h
	gcc -Wall -g -c $< -o $@ -D_GNU_SOURCE

mattrshare: mattrshare.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

mattrshare.o: mattrshare.c ../apue.h
	gcc -Wall -g -c $< -o $@

tconcurr: tconcurr.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

tconcurr.o: tconcurr.c ../apue.h
	gcc -Wall -g -c $< -o $@

tattrsize: tattrsize.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

tattrsize.o: tattrsize.c ../apue.h
	gcc -Wall -g -c $< -o $@

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp tattrsize tconcurr mattrshare mattrtype tattrtimeout rwattrshare cattrshare flock getenv static key cancel signal
	@echo "end clean"

.PHONY: clean
