all: tattrsize tconcurr mattrshare mattrtype tattrtimeout

tattrtimeout: tattrtimeout.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

tattrtimeout.o: tattrtimeout.c ../apue.h
	gcc -Wall -g -c $< -o $@ -D_GNU_SOURCE

mattrtype: mattrtype.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

mattrtype.o: mattrtype.c ../apue.h
	gcc -Wall -g -c $< -o $@ -D_GNU_SOURCE

mattrshare: mattrshare.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

mattrshare.o: mattrshare.c ../apue.h
	gcc -Wall -g -c $< -o $@

tconcurr: tconcurr.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

tconcurr.o: tconcurr.c ../apue.h
	gcc -Wall -g -c $< -o $@

tattrsize: tattrsize.o apue.o 
	gcc -Wall -g $^ -o $@ -pthread

tattrsize.o: tattrsize.c ../apue.h
	gcc -Wall -g -c $< -o $@

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp tattrsize tconcurr mattrshare mattrtype tattrtimeout
	@echo "end clean"

.PHONY: clean
