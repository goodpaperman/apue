all: print getopt

getopt: getopt.o apue.o
	gcc -Wall -g $^ -o $@

getopt.o: getopt.c ../apue.h
	gcc -Wall -g -c $< -o $@ 

print: print.o apue.o util.o
	gcc -Wall -g $^ -o $@

print.o: print.c print.h ../apue.h
	gcc -Wall -g -c $< -o $@ 

util.o: util.c print.h 
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD -DNO_LOG_TO_STDERR

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp print getopt
	@echo "end clean"

.PHONY: clean
