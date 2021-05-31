all:  setugid

setugid: setugid.o
	gcc -Wall -g $^ -o $@

setugid.o: setugid.c
	gcc -Wall -g -c $< -o $@

