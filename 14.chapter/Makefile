all: nonblock 

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
	-rm -f *.o core.* *.log *~ *.swp nonblock 
	@echo "end clean"

.PHONY: clean
