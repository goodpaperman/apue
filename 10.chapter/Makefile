all: coredump testsig

testsig: testsig.o apue.o
	gcc -Wall -g $^ -o $@

testsig.o: testsig.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

coredump: coredump.o apue.o
	gcc -Wall -g $^ -o $@

coredump.o: coredump.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp coredump testsig
	@echo "end clean"

.PHONY: clean