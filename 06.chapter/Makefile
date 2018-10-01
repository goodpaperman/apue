all: getpwnam getpwnam_ent getspnam getspnam_ent getgrnam getgrnam_ent getgrps setgrps gethostnam gethostnam_ent getnetnam getnetnam_ent getprotonam getprotonam_ent getservnam getservnam_ent uname utmp hostname time timeshift timeprintf 

timeprintf: timeprintf.o apue.o
	gcc -Wall -g $^ -o $@

timeprintf.o: timeprintf.c ../apue.h
	gcc -Wall -g -c $< -o $@

timeshift: timeshift.o apue.o
	gcc -Wall -g $^ -o $@

timeshift.o: timeshift.c ../apue.h
	gcc -Wall -g -c $< -o $@

time: time.o apue.o
	gcc -Wall -g $^ -o $@

time.o: time.c ../apue.h
	gcc -Wall -g -c $< -o $@

hostname: hostname.o apue.o
	gcc -Wall -g $^ -o $@

hostname.o: hostname.c ../apue.h
	gcc -Wall -g -c $< -o $@

utmp: utmp.o apue.o
	gcc -Wall -g $^ -o $@

utmp.o: utmp.c ../apue.h
	gcc -Wall -g -c $< -o $@

uname: uname.o apue.o
	gcc -Wall -g $^ -o $@

uname.o: uname.c ../apue.h
	gcc -Wall -g -c $< -o $@

getservnam_ent: getservnam_ent.o apue.o
	gcc -Wall -g $^ -o $@

getservnam_ent.o: getservnam_ent.c ../apue.h
	gcc -Wall -g -c $< -o $@

getservnam: getservnam.o apue.o
	gcc -Wall -g $^ -o $@

getservnam.o: getservnam.c ../apue.h
	gcc -Wall -g -c $< -o $@

getprotonam_ent: getprotonam_ent.o apue.o
	gcc -Wall -g $^ -o $@

getprotonam_ent.o: getprotonam_ent.c ../apue.h
	gcc -Wall -g -c $< -o $@

getprotonam: getprotonam.o apue.o
	gcc -Wall -g $^ -o $@

getprotonam.o: getprotonam.c ../apue.h
	gcc -Wall -g -c $< -o $@

getnetnam_ent: getnetnam_ent.o apue.o
	gcc -Wall -g $^ -o $@

getnetnam_ent.o: getnetnam_ent.c ../apue.h
	gcc -Wall -g -c $< -o $@

getnetnam: getnetnam.o apue.o
	gcc -Wall -g $^ -o $@

getnetnam.o: getnetnam.c ../apue.h
	gcc -Wall -g -c $< -o $@

gethostnam_ent: gethostnam_ent.o apue.o
	gcc -Wall -g $^ -o $@

gethostnam_ent.o: gethostnam_ent.c ../apue.h
	gcc -Wall -g -c $< -o $@

gethostnam: gethostnam.o apue.o
	gcc -Wall -g $^ -o $@

gethostnam.o: gethostnam.c ../apue.h
	gcc -Wall -g -c $< -o $@

getgrps: getgrps.o apue.o
	gcc -Wall -g $^ -o $@

getgrps.o: getgrps.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -D_POSIX_SOURCE

setgrps: setgrps.o apue.o
	gcc -Wall -g $^ -o $@

setgrps.o: setgrps.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -D_POSIX_SOURCE

getgrnam_ent: getgrnam_ent.o apue.o
	gcc -Wall -g $^ -o $@

getgrnam_ent.o: getgrnam_ent.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -D_POSIX_SOURCE

getgrnam: getgrnam.o apue.o
	gcc -Wall -g $^ -o $@

getgrnam.o: getgrnam.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -D_POSIX_SOURCE

getspnam_ent: getspnam_ent.o apue.o
	gcc -Wall -g $^ -o $@

getspnam_ent.o: getspnam_ent.c ../apue.h
	gcc -Wall -g -c $< -o $@

getspnam: getspnam.o apue.o
	gcc -Wall -g $^ -o $@

getspnam.o: getspnam.c ../apue.h
	gcc -Wall -g -c $< -o $@

getpwnam_ent: getpwnam_ent.o apue.o
	gcc -Wall -g $^ -o $@

getpwnam_ent.o: getpwnam_ent.c ../apue.h
	gcc -Wall -g -c $< -o $@

getpwnam: getpwnam.o apue.o
	gcc -Wall -g $^ -o $@

getpwnam.o: getpwnam.c ../apue.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp getpwnam getpwnam_ent getspnam getspnam_ent getgrnam getgrnam_ent getgrps setgrps gethostnam gethostnam_ent getnetnam getnetnam_ent getprotonam getprotonam_ent getservnam getservnam_ent uname utmp hostname time timeshift timeprintf
	@echo "end clean"

.PHONY: clean
