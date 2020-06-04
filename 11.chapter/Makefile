all: threadid threadexit threadcancel threaddetach threadstruct threadcleanup threadcleanup.i threadcleanup.ii threadmutex threaddeadlock threadmutex2 threadmutex3 threadrwlock threadrdlimit threadequal2 threadcond threadequal3 threadequal4

threadequal4: threadequal4.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadequal4.o: threadequal4.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadequal3: threadequal3.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadequal3.o: threadequal3.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadcond: threadcond.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadcond.o: threadcond.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadequal2: threadequal2.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadequal2.o: threadequal2.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadrdlimit: threadrdlimit.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadrdlimit.o: threadrdlimit.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadrwlock: threadrwlock.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadrwlock.o: threadrwlock.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadmutex3: threadmutex3.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadmutex3.o: threadmutex3.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadmutex2: threadmutex2.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadmutex2.o: threadmutex2.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threaddeadlock: threaddeadlock.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threaddeadlock.o: threaddeadlock.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadmutex: threadmutex.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadmutex.o: threadmutex.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadcleanup.ii: threadcleanup.c 
	g++ -Wall -g -E $^ -o $@

threadcleanup.i: threadcleanup.c 
	gcc -Wall -g -E $^ -o $@

threadcleanup: threadcleanup.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadcleanup.o: threadcleanup.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadstruct: threadstruct.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadstruct.o: threadstruct.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threaddetach: threaddetach.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threaddetach.o: threaddetach.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadcancel: threadcancel.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadcancel.o: threadcancel.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadexit: threadexit.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadexit.o: threadexit.c ../apue.h 
	gcc -Wall -g -c $< -o $@

threadid: threadid.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

threadid.o: threadid.c ../apue.h 
	gcc -Wall -g -c $< -o $@

#threadequal: threadequal.o apue.o 
#	g++ -Wall -g $^ -o $@ -pthread

#threadequal.o: threadequal.c ../apue.h concurrent_queue.hpp
#	g++ -Wall -g -c $< -o $@  -std=c++11 

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp *.i *.ii threadequal threadid threadexit threadcancel threaddetach threadstruct threadcleanup threadmutex threaddeadlock threadmutex2 threadmutex3 threadrwlock threadrdlimit threadequal2 threadcond threadequal3 threadequal4
	@echo "end clean"

.PHONY: clean
