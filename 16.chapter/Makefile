all: hostent addrinfo bind dgrams dgramc uptime_tcp_svc uptime_tcp_clt

uptime_tcp_clt: uptime_tcp_clt.o apue.o 
	gcc -Wall -g $^ -o $@

uptime_tcp_clt.o: uptime_tcp_clt.c ../apue.h
	gcc -Wall -g -c $< -o $@

uptime_tcp_svc: uptime_tcp_svc.o apue.o 
	gcc -Wall -g $^ -o $@

uptime_tcp_svc.o: uptime_tcp_svc.c ../apue.h
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
	-rm -f *.o core.* *.log *~ *.swp hostent addrinfo bind dgrams dgramc uptime_tcp_svc uptime_tcp_clt
	@echo "end clean"

.PHONY: clean
