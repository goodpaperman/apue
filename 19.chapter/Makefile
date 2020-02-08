all: term 

ttymode: ttymode.o tty.o print_lflag.o print_iflag.o print_cflag.o print_oflag.o apue.o
	gcc -Wall -g $^ -o $@

ttymode.o: ttymode.c tty.h ../apue.h
	gcc -Wall -g -c $< -o $@

tty.o: tty.c tty.h ../apue.h 
	gcc -Wall -g -c $< -o $@

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp 
	@echo "end clean"

.PHONY: clean
