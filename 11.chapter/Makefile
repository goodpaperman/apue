all: threadequal threadid threadexit

threadexit: threadexit.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadexit.o: threadexit.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadid: threadid.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadid.o: threadid.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadequal: threadequal.o apue.o 
	g++ -Wall -g $^ -o $@ -pthread

threadequal.o: threadequal.c ../apue.h concurrent_queue.hpp
	g++ -Wall -g -c $< -o $@  -std=c++11 

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp threadequal threadid threadexit
	@echo "end clean"

.PHONY: clean
