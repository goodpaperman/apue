all: padd2 add2 add5 padd5

padd5: padd5.o apue.o 
	gcc -Wall -g $^ -o $@

padd5.o: padd5.c ../apue.h
	gcc -Wall -g -c $< -o $@

add5: add5.o apue.o 
	gcc -Wall -g $^ -o $@

add5.o: add5.c ../apue.h
	gcc -Wall -g -c $< -o $@

add2: add2.o apue.o 
	gcc -Wall -g $^ -o $@

add2.o: add2.c ../apue.h
	gcc -Wall -g -c $< -o $@

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
	-rm -f *.o core.* *.log *~ *.swp padd2 add2 add5 padd5
	@echo "end clean"

.PHONY: clean
