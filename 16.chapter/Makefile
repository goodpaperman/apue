all: hostent 

hostent: hostent.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

hostent.o: hostent.c ../apue.h
	gcc -Wall -g -c $< -o $@

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp hostent 
	@echo "end clean"

.PHONY: clean
