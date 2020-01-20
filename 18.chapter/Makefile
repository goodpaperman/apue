all: padd2 

padd2: padd2.o apue.o 
	gcc -Wall -g $^ -o $@

padd2.o: padd2.c ../apue.h
	gcc -Wall -g -c $< -o $@

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp padd2 
	@echo "end clean"

.PHONY: clean
