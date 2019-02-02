all: nonblock filelock

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
	-rm -f *.o core.* *.log *~ *.swp nonblock filelock
	@echo "end clean"

.PHONY: clean
