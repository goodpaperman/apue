all: main atexit atexit_term echo env envop layout layout_s realloc envpos jump jumpvar jumpvar_opt rlimit lmt_as lmt_data lmt_core lmt_cpu lmt_fsize lmt_locks lmt_memlock lmt_msgque lmt_nofile lmt_nproc lmt_stack ret13 getarg 

getarg: getarg.o apue.o
	gcc -Wall -g $^ -o $@

getarg.o: getarg.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

ret13: ret13.o apue.o
	gcc -Wall -g $^ -o $@

ret13.o: ret13.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

lmt_stack: lmt_stack.o apue.o
	gcc -Wall -g $^ -o $@

lmt_stack.o: lmt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST_LIMIT_STACK

lmt_nproc: lmt_nproc.o apue.o
	gcc -Wall -g $^ -o $@

lmt_nproc.o: lmt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST_LIMIT_NPROC

lmt_nofile: lmt_nofile.o apue.o
	gcc -Wall -g $^ -o $@

lmt_nofile.o: lmt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST_LIMIT_NOFILE

lmt_msgque: lmt_msgque.o apue.o
	gcc -Wall -g $^ -o $@ -lrt

lmt_msgque.o: lmt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST_LIMIT_MSGQUEUE

lmt_memlock: lmt_memlock.o apue.o
	gcc -Wall -g $^ -o $@

lmt_memlock.o: lmt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST_LIMIT_MEMLOCK

lmt_locks: lmt_locks.o apue.o
	gcc -Wall -g $^ -o $@

lmt_locks.o: lmt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST_LIMIT_LOCKS

lmt_fsize: lmt_fsize.o apue.o
	gcc -Wall -g $^ -o $@

lmt_fsize.o: lmt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST_LIMIT_FSIZE

lmt_cpu: lmt_cpu.o apue.o
	gcc -Wall -g $^ -o $@

lmt_cpu.o: lmt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST_LIMIT_CPU

lmt_core: lmt_core.o apue.o
	gcc -Wall -g $^ -o $@

lmt_core.o: lmt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST_LIMIT_CORE

lmt_data: lmt_data.o apue.o
	gcc -Wall -g $^ -o $@

lmt_data.o: lmt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST_LIMIT_AS

lmt_as: lmt_as.o apue.o
	gcc -Wall -g $^ -o $@

lmt_as.o: lmt.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST_LIMIT_AS

rlimit: rlimit.o apue.o
	gcc -Wall -g $^ -o $@

rlimit.o: rlimit.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

jumpvar_opt: jumpvar_opt.o apue.o
	gcc -Wall -g $^ -o $@

jumpvar_opt.o: jumpvar.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99 -O

jumpvar: jumpvar.o apue.o
	gcc -Wall -g $^ -o $@

jumpvar.o: jumpvar.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

jump: jump.o apue.o
	gcc -Wall -g $^ -o $@

jump.o: jump.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

envpos: envpos.o apue.o
	gcc -Wall -g $^ -o $@

envpos.o: envpos.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

realloc: realloc.o apue.o
	gcc -Wall -g $^ -o $@

realloc.o: realloc.c ../apue.h
	gcc -Wall -g -c $< -o $@

layout_s: layout_s.o apue.o
	gcc -Wall -g $^ -o $@ -static

layout_s.o: layout.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

layout: layout.o apue.o
	gcc -Wall -g $^ -o $@ -ldl

layout.o: layout.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

envop: envop.o apue.o
	gcc -Wall -g $^ -o $@

envop.o: envop.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

env: env.o apue.o
	gcc -Wall -g $^ -o $@

env.o: env.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

echo: echo.o apue.o
	gcc -Wall -g $^ -o $@

echo.o: echo.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

atexit: atexit.o apue.o
	gcc -Wall -g $^ -o $@

atexit.o: atexit.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

atexit_term: atexit_term.o apue.o
	gcc -Wall -g $^ -o $@

atexit_term.o: atexit_term.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

main: main.o apue.o
	gcc -Wall -g $^ -o $@

main.o: main.c ../apue.h
	gcc -Wall -g -c $< -o $@ -std=c99

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp main atexit atexit_term echo env envop layout layout_s realloc envpos jump jumpvar jumpvar_opt rlimit lmt_as lmt_data lmt_core lmt_cpu lmt_fsize lmt_locks lmt_memlock lmt_msgque lmt_nofile lmt_nproc lmt_stack ret13 getarg
	@echo "end clean"

.PHONY: clean
