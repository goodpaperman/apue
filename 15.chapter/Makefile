all: pipestat pipemulti

pipemulti: pipemulti.o apue.o 
	gcc -Wall -g $^ -o $@

pipemulti.o: pipemulti.c ../apue.h
	gcc -Wall -g -c $< -o $@

pipestat: pipestat.o apue.o 
	gcc -Wall -g $^ -o $@

pipestat.o: pipestat.c ../apue.h
	gcc -Wall -g -c $< -o $@

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp pipestat pipemulti
	@echo "end clean"

.PHONY: clean
