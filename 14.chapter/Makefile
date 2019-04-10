all: nonblock filelock flocksplit flockcover flockdead flockdeadpair filelockfd flockend flockmandatory flockoccupy sigio linu_writev

linu_writev: linu_writev.o
	gcc -Wall -g $^ -o $@

linu_writev.o: linu_writev.c
	gcc -Wall -g -c $< -o $@

sigio: sigio.o apue.o 
	gcc -Wall -g $^ -o $@

sigio.o: sigio.c ../apue.h
	gcc -Wall -g -c $< -o $@

flockoccupy: flockoccupy.o apue.o 
	gcc -Wall -g $^ -o $@

flockoccupy.o: flockoccupy.c ../apue.h
	gcc -Wall -g -c $< -o $@

flockmandatory: flockmandatory.o apue.o 
	gcc -Wall -g $^ -o $@

flockmandatory.o: flockmandatory.c ../apue.h
	gcc -Wall -g -c $< -o $@

flockend: flockend.o apue.o 
	gcc -Wall -g $^ -o $@

flockend.o: flockend.c ../apue.h
	gcc -Wall -g -c $< -o $@

filelockfd: filelockfd.o apue.o 
	gcc -Wall -g $^ -o $@

filelockfd.o: filelockfd.c ../apue.h
	gcc -Wall -g -c $< -o $@

flockdeadpair: flockdeadpair.o apue.o 
	gcc -Wall -g $^ -o $@

flockdeadpair.o: flockdeadpair.c ../apue.h
	gcc -Wall -g -c $< -o $@

flockdead: flockdead.o apue.o 
	gcc -Wall -g $^ -o $@

flockdead.o: flockdead.c ../apue.h
	gcc -Wall -g -c $< -o $@

flockcover: flockcover.o apue.o 
	gcc -Wall -g $^ -o $@

flockcover.o: flockcover.c ../apue.h
	gcc -Wall -g -c $< -o $@

flocksplit: flocksplit.o apue.o 
	gcc -Wall -g $^ -o $@

flocksplit.o: flocksplit.c ../apue.h
	gcc -Wall -g -c $< -o $@

filelock: filelock.o apue.o 
	gcc -Wall -g $^ -o $@

filelock.o: filelock.c ../apue.h
	gcc -Wall -g -c $< -o $@

nonblock: nonblock.o apue.o 
	gcc -Wall -g $^ -o $@

nonblock.o: nonblock.c ../apue.h
	gcc -Wall -g -c $< -o $@

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp nonblock filelock flocksplit flockcover flockdead flockdeadpair filelockfd flockend flockmandatory flockoccupy sigio linu_writev
	@echo "end clean"

.PHONY: clean
