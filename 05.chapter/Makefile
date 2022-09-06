all: fwide fwidew fsetbuf fgetbuf freopen fdopen fdopen_t finout fgetc fungetc fgets fputs fcopyc fcopys fseek fseeko32 fseeko64 fprintf fsprintf fscanf pr_stdio tmpnam mktemp fflushline

fwide: fwide.o apue.o
	gcc -Wall -g $^ -o $@

fwide.o: fwide.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -DUSE_EXPLICIT_FWIDE

fwidew: fwidew.o apue.o
	gcc -Wall -g $^ -o $@

fwidew.o: fwide.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -DUSE_WCHAR -DUSE_EXPLICIT_FWIDE

fsetbuf: fsetbuf.o apue.o
	gcc -Wall -g $^ -o $@

fsetbuf.o: fsetbuf.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -std=c99

fgetbuf: fgetbuf.o apue.o
	gcc -Wall -g $^ -o $@

fgetbuf.o: fgetbuf.c ../apue.h 
	gcc -Wall -g -c $< -o $@

freopen: freopen.o apue.o
	gcc -Wall -g $^ -o $@

freopen.o: freopen.c ../apue.h 
	gcc -Wall -g -c $< -o $@

fdopen: fdopen.o apue.o
	gcc -Wall -g $^ -o $@

fdopen.o: fdopen.c ../apue.h 
	gcc -Wall -g -c $< -o $@

fdopen_t: fdopen_t.o apue.o
	gcc -Wall -g $^ -o $@

fdopen_t.o: fdopen_t.c ../apue.h 
	gcc -Wall -g -c $< -o $@

finout: finout.o apue.o
	gcc -Wall -g $^ -o $@

finout.o: finout.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -std=c99 -DTEST1 -DTEST2 -DTEST3 -DTEST4 -DTEST5

fgetc: fgetc.o apue.o
	gcc -Wall -g $^ -o $@

fgetc.o: fgetc.c ../apue.h 
	gcc -Wall -g -c $< -o $@

fungetc: fungetc.o apue.o
	gcc -Wall -g $^ -o $@

fungetc.o: fungetc.c ../apue.h 
	gcc -Wall -g -c $< -o $@

fgets: fgets.o apue.o
	gcc -Wall -g $^ -o $@

fgets.o: fgets.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -DUSE_GETS #-DENOUGH_BUF 

fputs: fputs.o apue.o
	gcc -Wall -g $^ -o $@

fputs.o: fputs.c ../apue.h 
	gcc -Wall -g -c $< -o $@ 

fcopyc: fcopyc.o apue.o
	gcc -Wall -g $^ -o $@

fcopyc.o: fputc.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -DUSE_F

fcopys: fcopys.o apue.o
	gcc -Wall -g $^ -o $@

fcopys.o: fputc.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -DUSE_S

fseek: fseek.o apue.o
	gcc -Wall -g $^ -o $@

fseek.o: fseek.c ../apue.h 
	gcc -Wall -g -c $< -o $@

fseeko32: fseeko32.o apue.o 
	gcc -Wall -g $^ -o $@

fseeko32.o: fseeko.c ../apue.h 
	gcc -Wall -g -c $< -o $@ #-D_FILE_OFFSET_BITS=32

fseeko64: fseeko64.o apue.o
	gcc -Wall -g $^ -o $@

fseeko64.o: fseeko.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE

fprintf: fprintf.o apue.o
	gcc -Wall -g $^ -o $@

fprintf.o: fprintf.c ../apue.h 
	gcc -Wall -g -c $< -o $@

fsprintf: fsprintf.o apue.o
	gcc -Wall -g $^ -o $@

fsprintf.o: fsprintf.c ../apue.h 
	gcc -Wall -g -c $< -o $@

fscanf: fscanf.o apue.o
	gcc -Wall -g $^ -o $@

fscanf.o: fscanf.c ../apue.h 
	gcc -Wall -g -c $< -o $@

pr_stdio: pr_stdio.o apue.o
	gcc -Wall -g $^ -o $@

pr_stdio.o: pr_stdio.c ../apue.h 
	gcc -Wall -g -c $< -o $@

tmpnam: tmpnam.o apue.o
	gcc -Wall -g $^ -o $@

tmpnam.o: tmpnam.c ../apue.h 
	gcc -Wall -g -c $< -o $@

mktemp: mktemp.o apue.o
	gcc -Wall -g $^ -o $@

mktemp.o: mktemp.c ../apue.h 
	gcc -Wall -g -c $< -o $@

fflushline: fflushline.o apue.o
	gcc -Wall -g $^ -o $@

fflushline.o: fflushline.c ../apue.h 
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp fwide fwidew fsetbuf fgetbuf freopen fdopen fdopen_t finout fgetc fungetc fgets fputs fcopyc fcopys fseek fseeko32 fseeko64 fprintf fsprintf fscanf pr_stdio tmpnam mktemp fflushline
	@echo "end clean"

.PHONY: clean
