all: name_max path_max 

name_max: name_max.o apue.o
	gcc -Wall -g $^ -o $@

name_max.o: name_max.c ../apue.h 
	gcc -Wall -g -c $< -o $@

path_max: path_max.o apue.o
	gcc -Wall -g $^ -o $@

path_max.o: path_max.c ../apue.h 
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp name_max path_max 
	@echo "end clean"

.PHONY: clean
