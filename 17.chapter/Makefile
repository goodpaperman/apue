all: padd2 add2 add5 padd5 spipe_server spipe_client csopenc csopend csopenc2 csopend2

csopend2: csopend2.o csopen2.o spipe.o spipe_fd.o apue.o
	gcc -Wall -g $^ -o $@

csopend2.o: csopend2.c csopen2.h spipe.h spipe_fd.h ../apue.h
	gcc -Wall -g -c $< -o $@

csopenc2: csopenc2.o csopen2.o spipe.o spipe_fd.o apue.o
	gcc -Wall -g $^ -o $@

csopenc2.o: csopenc.c csopen2.h spipe.h spipe_fd.h ../apue.h
	gcc -Wall -g -c $< -o $@

csopen2.o: csopen2.c csopen2.h ../apue.h
	gcc -Wall -g -c $< -o $@

csopend: csopend.o spipe_fd.o apue.o
	gcc -Wall -g $^ -o $@

csopend.o: csopend.c spipe_fd.h ../apue.h
	gcc -Wall -g -c $< -o $@

csopenc: csopenc.o csopen.o spipe_fd.o apue.o
	gcc -Wall -g $^ -o $@

csopenc.o: csopenc.c csopen.h spipe_fd.h ../apue.h
	gcc -Wall -g -c $< -o $@

csopen.o: csopen.c csopen.h ../apue.h
	gcc -Wall -g -c $< -o $@

spipe_client: spipe_client.o spipe_fd.o
	gcc -Wall -g $^ -o $@

spipe_client.o: spipe_client.c 
	gcc -Wall -g -c $< -o $@

spipe_server: spipe_server.o spipe_fd.o
	gcc -Wall -g $^ -o $@

spipe_server.o: spipe_server.c 
	gcc -Wall -g -c $< -o $@

spipe_fd.o: spipe_fd.c 
	gcc -Wall -g -c $< -o $@ -D_GNU_SOURCE

padd5: padd5.o spipe.o apue.o 
	gcc -Wall -g $^ -o $@

padd5.o: padd5.c spipe.h ../apue.h
	gcc -Wall -g -c $< -o $@

add5: add5.o spipe.o apue.o 
	gcc -Wall -g $^ -o $@

add5.o: add5.c spipe.h ../apue.h
	gcc -Wall -g -c $< -o $@

spipe.o: spipe.c spipe.h 
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
	-rm -f *.o core.* *.log *~ *.swp padd2 add2 add5 padd5 spipe_server spipe_client csopenc csopend csopenc2
	@echo "end clean"

.PHONY: clean
