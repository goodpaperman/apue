all: grpid grpmaster grpslave grpmaste grpmast grpslav grpmas grpsla session tty pty telnetd telnets telnety

telnety: telnety.o apue.o
	gcc -Wall -g $^ -o $@ -lutil

telnety.o: telnet/telnety.c 
	gcc -Wall -g -c $< -o $@ #-std=c99

telnets: telnets.o pty_fun.o apue.o
	gcc -Wall -g $^ -o $@

telnets.o: telnet/telnets.c telnet/pty_fun.h
	gcc -Wall -g -c $< -o $@ #-std=c99

telnetd: telnetd.o pty_fun.o apue.o
	gcc -Wall -g $^ -o $@

telnetd.o: telnet/telnetd.c telnet/pty_fun.h
	gcc -Wall -g -c $< -o $@ #-std=c99

pty_fun.o: telnet/pty_fun.c telnet/pty_fun.h
	gcc -Wall -g -c $< -o $@ -D_GNU_SOURCE #-std=c99

pty: pty.o apue.o
	gcc -Wall -g $^ -o $@ -lutil

pty.o: pty.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

tty: tty.o apue.o
	gcc -Wall -g $^ -o $@

tty.o: tty.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

session: session.o apue.o
	gcc -Wall -g $^ -o $@

session.o: session.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

grpsla: grpsla.o apue.o
	gcc -Wall -g $^ -o $@

grpsla.o: grpsla.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

grpmas: grpmas.o apue.o
	gcc -Wall -g $^ -o $@

grpmas.o: grpmas.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DUSE_SYSTEM

grpslav: grpslav.o apue.o
	gcc -Wall -g $^ -o $@

grpslav.o: grpslav.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

grpmast: grpmast.o apue.o
	gcc -Wall -g $^ -o $@

grpmast.o: grpmast.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DUSE_SYSTEM

grpmaste: grpmaste.o apue.o
	gcc -Wall -g $^ -o $@

grpmaste.o: grpmaste.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 #-DUSE_SYSTEM

grpslave: grpslave.o apue.o
	gcc -Wall -g $^ -o $@

grpslave.o: grpslave.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

grpmaster: grpmaster.o apue.o
	gcc -Wall -g $^ -o $@

grpmaster.o: grpmaster.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 #-DUSE_SYSTEM

grpid: grpid.o apue.o
	gcc -Wall -g $^ -o $@

grpid.o: grpid.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp grpid grpmaster grpslave grpmaste grpmast grpslav grpmas grpsla session tty pty telnetd telnets telnety
	@echo "end clean"

.PHONY: clean
