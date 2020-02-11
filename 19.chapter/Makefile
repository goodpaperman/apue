all: pty 

pty: pty.o tty.o apue.o pty_fun.o
	gcc -Wall -g $^ -o $@

pty.o: pty.c ../tty.h ../pty_fun.h ../apue.h
	gcc -Wall -g -c $< -o $@

tty.o: ../tty.c ../tty.h ../apue.h 
	gcc -Wall -g -c $< -o $@

pty_fun.o: ../pty_fun.c ../pty_fun.h ../apue.h 
	gcc -Wall -g -c $< -o $@

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp 
	@echo "end clean"

.PHONY: clean
