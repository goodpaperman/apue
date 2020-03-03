all: pty slowout add2 padd2

padd2: padd2.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

padd2.o: ../15.chapter/padd2.c ../apue.h
	gcc -Wall -g -c $< -o $@ -DUSE_PTY

add2: add2.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

add2.o: ../15.chapter/add3.c ../apue.h
	gcc -Wall -g -c $< -o $@ 

slowout: slowout.c 
	gcc -Wall -g $^ -o $@

pty: pty.o tty.o apue.o pty_fun.o
	gcc -Wall -g $^ -o $@

pty.o: pty.c ../tty.h ../pty_fun.h ../apue.h
	gcc -Wall -g -c $< -o $@ -DUSE_POLL
#gcc -Wall -g -c $< -o $@ -DUSE_SIGTERM

tty.o: ../tty.c ../tty.h ../apue.h 
	gcc -Wall -g -c $< -o $@

pty_fun.o: ../pty_fun.c ../pty_fun.h ../apue.h 
	gcc -Wall -g -c $< -o $@ -D_XOPEN_SOURCE

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp pty slowout add2 padd2
	@echo "end clean"

.PHONY: clean
