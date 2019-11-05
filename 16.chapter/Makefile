all: hostent addrinfo bind dgrams dgramc uptime_svc_oob uptime_clt uptime_svc_io

uptime_svc_io: uptime_svc_io.o apue.o 
	gcc -Wall -g $^ -o $@

uptime_svc_io.o: uptime_svc_io.c ../apue.h
	gcc -Wall -g -c $< -o $@

uptime_clt: uptime_clt.o apue.o 
	gcc -Wall -g $^ -o $@

uptime_clt.o: uptime_clt.c ../apue.h
	gcc -Wall -g -c $< -o $@

uptime_svc_oob: uptime_svc_oob.o apue.o 
	gcc -Wall -g $^ -o $@

uptime_svc_oob.o: uptime_svc_oob.c ../apue.h
	gcc -Wall -g -c $< -o $@

dgramc: dgramc.o apue.o 
	gcc -Wall -g $^ -o $@

dgramc.o: dgramc.c ../apue.h
	gcc -Wall -g -c $< -o $@

dgrams: dgrams.o apue.o 
	gcc -Wall -g $^ -o $@

dgrams.o: dgrams.c ../apue.h
	gcc -Wall -g -c $< -o $@

bind: bind.o apue.o 
	gcc -Wall -g $^ -o $@

bind.o: bind.c ../apue.h
	gcc -Wall -g -c $< -o $@

addrinfo: addrinfo.o apue.o 
	gcc -Wall -g $^ -o $@

addrinfo.o: addrinfo.c ../apue.h
	gcc -Wall -g -c $< -o $@

hostent: hostent.o apue.o 
	gcc -Wall -g $^ -o $@

hostent.o: hostent.c ../apue.h
	gcc -Wall -g -c $< -o $@

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp hostent addrinfo bind dgrams dgramc uptime_svc_oob uptime_clt uptime_svc_io
	@echo "end clean"

.PHONY: clean
