all: pipestat pipemulti pipeatom pipepoll pipepage ppage pmulti

pmulti: pmulti.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

pmulti.o: pmulti.c ../apue.h
	gcc -Wall -g -c $< -o $@

ppage: ppage.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

ppage.o: ppage.c ../apue.h
	gcc -Wall -g -c $< -o $@

pipepage: pipepage.o apue.o 
	gcc -Wall -g $^ -o $@

pipepage.o: pipepage.c ../apue.h
	gcc -Wall -g -c $< -o $@

pipepoll: pipepoll.o apue.o 
	gcc -Wall -g $^ -o $@

pipepoll.o: pipepoll.c ../apue.h
	gcc -Wall -g -c $< -o $@

pipeatom: pipeatom.o apue.o 
	gcc -Wall -g $^ -o $@

pipeatom.o: pipeatom.c ../apue.h
	gcc -Wall -g -c $< -o $@

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
	gcc -Wall -g -c $< -o $@ -D__USE_BSD -DUSE_PTHREAD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp pipestat pipemulti pipeatom pipepoll pipepage ppage
	@echo "end clean"

.PHONY: clean
