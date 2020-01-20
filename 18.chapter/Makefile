all: term 

term: term.o print_iflag.o print_oflag.o print_cflag.o print_lflag.o print_cchar.o apue.o 
	gcc -Wall -g $^ -o $@

term.o: term.c ../apue.h
	gcc -Wall -g -c $< -o $@

print_iflag.o: print_iflag.c ../apue.h
	gcc -Wall -g -c $< -o $@

print_iflag.c: print_flag.awk iflag.sym
	./print_flag.awk -v FUNC_NAME=input -v MACRO_FILE=iflag.sym > print_iflag.c

print_oflag.o: print_oflag.c ../apue.h
	gcc -Wall -g -c $< -o $@

print_oflag.c: print_flag.awk oflag.sym
	./print_flag.awk -v FUNC_NAME=output -v MACRO_FILE=oflag.sym > print_oflag.c

print_cflag.o: print_cflag.c ../apue.h
	gcc -Wall -g -c $< -o $@

print_cflag.c: print_flag.awk cflag.sym
	awk -v FUNC_NAME=control -v MACRO_FILE=cflag.sym -f print_flag.awk > print_cflag.c

print_lflag.o: print_lflag.c ../apue.h
	gcc -Wall -g -c $< -o $@

print_lflag.c: print_flag.awk lflag.sym
	./print_flag.awk -v FUNC_NAME=local -v MACRO_FILE=lflag.sym > print_lflag.c

print_cchar.o: print_cchar.c ../apue.h
	gcc -Wall -g -c $< -o $@

print_cchar.c: print_char.awk cchar.sym
	./print_char.awk > print_cchar.c

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp term 
	@echo "end clean"

.PHONY: clean
