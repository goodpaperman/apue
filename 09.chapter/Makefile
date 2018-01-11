all: grpid grpmaster grpslave grpmaste grpmast grpslav

grpslav: grpslav.o apue.o
	gcc -Wall -g $^ -o $@

grpslav.o: grpslav.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

grpmast: grpmast.o apue.o
	gcc -Wall -g $^ -o $@

grpmast.o: grpmast.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DUSE_SYSTEM

grpmaste: grpmaste.o apue.o
	gcc -Wall -g $^ -o $@

grpmaste.o: grpmaste.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 #-DUSE_SYSTEM

grpslave: grpslave.o apue.o
	gcc -Wall -g $^ -o $@

grpslave.o: grpslave.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

grpmaster: grpmaster.o apue.o
	gcc -Wall -g $^ -o $@

grpmaster.o: grpmaster.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 #-DUSE_SYSTEM

grpid: grpid.o apue.o
	gcc -Wall -g $^ -o $@

grpid.o: grpid.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp grpid grpmaster grpslave grpmaste grpmast grpslav
	@echo "end clean"

.PHONY: clean
