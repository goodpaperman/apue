all: grpid grpmaste grpslave grpmaster

grpmaster: grpmaster.o apue.o
	gcc -Wall -g $^ -o $@

grpmaster.o: grpmaster.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 #-DUSE_SYSTEM

grpslave: grpslave.o apue.o
	gcc -Wall -g $^ -o $@

grpslave.o: grpslave.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

grpmaste: grpmaste.o apue.o
	gcc -Wall -g $^ -o $@

grpmaste.o: grpmaste.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 #-DUSE_SYSTEM

grpid: grpid.o apue.o
	gcc -Wall -g $^ -o $@

grpid.o: grpid.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp grpid grpmaste grpslave grpmaster
	@echo "end clean"

.PHONY: clean
