all: creat seekable seekhole read_inter write_seek write_limit cloexec_i cloexec dup_fd sync_fd print_fd copy copy2 copy3 copy4 copy5 

creat: creat.o
	gcc -Wall -g $^ -o $@

creat.o: creat.c
	gcc -Wall -g -c $< -o $@

seekable: seekable.o
	gcc -Wall -g $^ -o $@

seekable.o: seekable.c
	gcc -Wall -g -c $< -o $@

seekhole: seekhole.o apue.o
	gcc -Wall -g $^ -o $@

seekhole.o: seekhole.c ../apue.h 
	gcc -Wall -g -c $< -o $@

read_inter: read_inter.o apue.o
	gcc -Wall -g $^ -o $@

read_inter.o: read_inter.c ../apue.h 
	gcc -Wall -g -c $< -o $@

write_seek: write_seek.o apue.o
	gcc -Wall -g $^ -o $@

write_seek.o: write_seek.c ../apue.h 
	gcc -Wall -g -c $< -o $@

write_limit: write_limit.o apue.o
	gcc -Wall -g $^ -o $@

write_limit.o: write_limit.c ../apue.h 
	gcc -Wall -g -c $< -o $@

cloexec_i: cloexec_i.o apue.o
	gcc -Wall -g $^ -o $@

cloexec_i.o: cloexec_i.c ../apue.h
	gcc -Wall -g -c $< -o $@

cloexec: cloexec.o apue.o
	gcc -Wall -g $^ -o $@

cloexec.o: cloexec.c ../apue.h
	gcc -Wall -g -c $< -o $@ -DUSE_CLOEXEC -D_GNU_SOURCE

dup_fd: dup_fd.o apue.o
	gcc -Wall -g $^ -o $@

dup_fd.o: dup_fd.c ../apue.h
	gcc -Wall -g -c $< -o $@ -D_GNU_SOURCE

sync_fd: sync_fd.o apue.o
	gcc -Wall -g $^ -o $@

sync_fd.o: sync_fd.c ../apue.h
	gcc -Wall -g -c $< -o $@

print_fd: print_fd.o apue.o
	gcc -Wall -g $^ -o $@

print_fd.o: print_fd.c ../apue.h
	gcc -Wall -g -c $< -o $@

copy: copy.o apue.o
	gcc -Wall -g $^ -o $@

copy.o: copy.c ../apue.h 
	gcc -Wall -g -c $< -o $@

copy2: copy2.o apue.o
	gcc -Wall -g $^ -o $@

copy2.o: copy.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -DUSE_O_SYNC

copy3: copy3.o apue.o
	gcc -Wall -g $^ -o $@

copy3.o: copy.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -DUSE_DATASYNC

copy4: copy4.o apue.o
	gcc -Wall -g $^ -o $@

copy4.o: copy.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -DUSE_SYNC

copy5: copy5.o apue.o
	gcc -Wall -g $^ -o $@

copy5.o: copy.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -DUSE_SYNC -DUSE_O_SYNC

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp creat seekable seekhole read_inter write_seek write_limit cloexec_i cloexec dup_fd sync_fd print_fd copy copy2 copy3 copy4 copy5 
	@echo "end clean"

.PHONY: clean
