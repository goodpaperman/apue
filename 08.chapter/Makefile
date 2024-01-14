all: clonefd forkfd forkit fork fork_stdout fork_nproc vfork vforkex vforkest wait waitpid waitid waitusage prexit daemon race exec_on_close exec_no_close exec_open_dir exec_atexit exec_pthread fork_pthread pid exec_signal exec_mmap exec_sem exec execp echoall uucp tip pit tpi pti system login libhack.so checkin vacct oacct loginname proctime childtime tsys zombie memtrans
	
memtrans: memtrans.o
	gcc -Wall -g $^ -o $@

memtrans.o: memtrans.c
	gcc -Wall -g -c $< -o $@ -std=c99


zombie: zombie.o apue.o
	gcc -Wall -g $^ -o $@

zombie.o: zombie.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -D_BSD_SOURCE

tsys: tsys.o apue.o
	gcc -Wall -g $^ -o $@

tsys.o: tsys.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

childtime: childtime.o apue.o
	gcc -Wall -g $^ -o $@

childtime.o: childtime.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

proctime: proctime.o apue.o
	gcc -Wall -g $^ -o $@

proctime.o: proctime.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

loginname: loginname.o apue.o
	gcc -Wall -g $^ -o $@

loginname.o: loginname.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

oacct: oacct.o apue.o
	gcc -Wall -g $^ -o $@

oacct.o: oacct.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -D_POSIX_SOURCE

vacct: vacct.o apue.o
	gcc -Wall -g $^ -o $@

vacct.o: vacct.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -D_POSIX_SOURCE

checkin: checkin.o apue.o
	gcc -Wall -g -L. -lhack -Wl,-rpath,./ $^ -o $@

checkin.o: login.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

libhack.so: hack.o 
	gcc -Wall -g -shared -fPIC $^ -o $@

hack.o: hack.c 
	gcc -Wall -g -fPIC -c $< -o $@ -std=c99

login: login.o apue.o
	gcc -Wall -g $^ -o $@

login.o: login.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

system: system.o apue.o system_api.o
	gcc -Wall -g $^ -o $@

system.o: system.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

system_api.o: system_api.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

pti: pti.o apue.o
	gcc -Wall -g $^ -o $@

pti.o: pti.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

tpi: tpi.o apue.o
	gcc -Wall -g $^ -o $@

tpi.o: tpi.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

pit: pit.o apue.o
	gcc -Wall -g $^ -o $@

pit.o: pit.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

tip: tip.o apue.o
	gcc -Wall -g $^ -o $@

tip.o: tip.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

uucp: uucp.o apue.o
	gcc -Wall -g $^ -o $@

uucp.o: uucp.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

echoall: echoall.o apue.o
	gcc -Wall -g $^ -o $@

echoall.o: echoall.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

execp: execp.o apue.o
	gcc -Wall -g $^ -o $@

execp.o: execp.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

exec: exec.o apue.o
	gcc -Wall -g $^ -o $@

exec.o: exec.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

exec_sem: exec_sem.o apue.o
	gcc -Wall -g $^ -o $@ -pthread

exec_sem.o: exec_sem.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

exec_mmap: exec_mmap.o apue.o
	gcc -Wall -g $^ -o $@

exec_mmap.o: exec_mmap.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

exec_signal: exec_signal.o apue.o
	gcc -Wall -g $^ -o $@

exec_signal.o: exec_signal.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -D_GNU_SOURCE

exec_pthread: exec_pthread.o apue.o
	gcc -Wall -g $^ -o $@ -pthread

exec_pthread.o: exec_pthread.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

fork_pthread: fork_pthread.o apue.o
	gcc -Wall -g $^ -o $@ -pthread

fork_pthread.o: fork_pthread.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 #-DFORK_IN_THREAD

pid: pid.o
	g++ -Wall -g $^ -o $@ 

pid.o: pid.c
	g++ -Wall -g -c $< -o $@ 

exec_atexit: exec_atexit.o apue.o
	gcc -Wall -g $^ -o $@

exec_atexit.o: exec_atexit.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

exec_open_dir: exec_open_dir.o apue.o
	gcc -Wall -g $^ -o $@

exec_open_dir.o: exec_open_dir.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

exec_no_close: exec_no_close.o apue.o
	gcc -Wall -g $^ -o $@

exec_no_close.o: exec_on_close.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

exec_on_close: exec_on_close.o apue.o
	gcc -Wall -g $^ -o $@

exec_on_close.o: exec_on_close.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DCLOSE_ON_EXEC -D_GNU_SOURCE

race: race.o apue.o
	gcc -Wall -g $^ -o $@

race.o: race.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

daemon: daemon.o apue.o
	gcc -Wall -g $^ -o $@

daemon.o: daemon.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

prexit: prexit.o apue.o
	gcc -Wall -g $^ -o $@

prexit.o: prexit.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -D__USE_BSD

waitusage: waitusage.o apue.o
	gcc -Wall -g $^ -o $@

waitusage.o: wait.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DUSE_WAIT_RUSAGE -D__USE_BSD

waitid: waitid.o apue.o
	gcc -Wall -g $^ -o $@

waitid.o: wait.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DUSE_WAITID -D__USE_BSD

waitpid: waitpid.o apue.o
	gcc -Wall -g $^ -o $@

waitpid.o: wait.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DUSE_WAITPID -D__USE_BSD

wait: wait.o apue.o
	gcc -Wall -g $^ -o $@

wait.o: wait.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -D__USE_BSD

vforkest: vforkest.o apue.o
	gcc -Wall -g $^ -o $@

vforkest.o: vforkest.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

vforkex: vforkex.o apue.o
	gcc -Wall -g $^ -o $@

vforkex.o: vforkex.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

vfork: vfork.o apue.o
	gcc -Wall -g $^ -o $@

vfork.o: vfork.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

fork_nproc: fork_nproc.o apue.o
	gcc -Wall -g $^ -o $@

fork_nproc.o: fork_nproc.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

fork_stdout: fork_stdout.o apue.o
	gcc -Wall -g $^ -o $@

fork_stdout.o: fork_stdout.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -D__USE_BSD

fork: fork.o apue.o
	gcc -Wall -g $^ -o $@

fork.o: fork.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

forkit: forkit.o
	gcc -Wall -g $^ -o $@

forkit.o: forkit.c
	gcc -Wall -g -c $< -o $@ -std=c99 # -DUSE_VFORK

forkfd: forkfd.o
	gcc -Wall -g $^ -o $@

forkfd.o: forkfd.c
	gcc -Wall -g -c $< -o $@ -std=c99 -DUSE_VFORK

clonefd: clonefd.o
	gcc -Wall -g $^ -o $@

clonefd.o: clonefd.c
	gcc -Wall -g -c $< -o $@ -std=c99 -D_GNU_SOURCE

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp clonefd forkfd forkit fork fork_stdout fork_nproc vfork vforkex vforkest wait waitpid waitid waitusage prexit daemon race exec_on_close exec_no_close exec_open_dir exec_atexit exec_pthread fork_pthread pid exec_signal exec_mmap exec_sem exec execp echoall uucp tip pit tpi pti system login libhack.so checkin vacct oacct loginname proctime childtime tsys zombie memtrans
	@echo "end clean"

.PHONY: clean
