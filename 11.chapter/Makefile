all: threadequal 

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
	-rm -f *.o core.* *.log *~ *.swp threadequal 
	@echo "end clean"

.PHONY: clean
