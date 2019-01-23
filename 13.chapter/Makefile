all: daemon sighup

sighup: sighup.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

sighup.o: sighup.c ../apue.h
	gcc -Wall -g -c $< -o $@

daemon: daemon.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

daemon.o: daemon.c ../apue.h
	gcc -Wall -g -c $< -o $@

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp daemon sighup
	@echo "end clean"

.PHONY: clean
