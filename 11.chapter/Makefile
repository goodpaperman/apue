all: threadid 

threadid: threadid.o apue.o 
	g++ -Wall -g $^ -o $@ -pthread

threadid.o: threadid.c ../apue.h concurrent_queue.hpp
	g++ -Wall -g -c $< -o $@  -std=c++11 

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp threadid 
	@echo "end clean"

.PHONY: clean
