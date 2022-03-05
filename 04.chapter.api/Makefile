all: mkdir_api open_api link_api symlink_api readlink_api unlink_api remove_api rmdir_api rename_api write_api

mkdir_api: mkdir_api.o apue.o
	gcc -Wall -g $^ -o $@ 

mkdir_api.o: mkdir_api.c ../apue.h 
	gcc -Wall -g -c $< -o $@

open_api: open_api.o apue.o
	gcc -Wall -g $^ -o $@ 

open_api.o: open_api.c ../apue.h 
	gcc -Wall -g -c $< -o $@

link_api: link_api.o apue.o
	gcc -Wall -g $^ -o $@ 

link_api.o: link_api.c ../apue.h 
	gcc -Wall -g -c $< -o $@

symlink_api: symlink_api.o apue.o
	gcc -Wall -g $^ -o $@ 

symlink_api.o: symlink_api.c ../apue.h 
	gcc -Wall -g -c $< -o $@

readlink_api: readlink_api.o apue.o
	gcc -Wall -g $^ -o $@ 

readlink_api.o: readlink_api.c ../apue.h 
	gcc -Wall -g -c $< -o $@

unlink_api: unlink_api.o apue.o
	gcc -Wall -g $^ -o $@ 

unlink_api.o: unlink_api.c ../apue.h 
	gcc -Wall -g -c $< -o $@

remove_api: remove_api.o apue.o
	gcc -Wall -g $^ -o $@ 

remove_api.o: remove_api.c ../apue.h 
	gcc -Wall -g -c $< -o $@

rmdir_api: rmdir_api.o apue.o
	gcc -Wall -g $^ -o $@ 

rmdir_api.o: rmdir_api.c ../apue.h 
	gcc -Wall -g -c $< -o $@

rename_api: rename_api.o apue.o
	gcc -Wall -g $^ -o $@ 

rename_api.o: rename_api.c ../apue.h 
	gcc -Wall -g -c $< -o $@

write_api: write_api.o apue.o
	gcc -Wall -g $^ -o $@ 

write_api.o: write_api.c ../apue.h 
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp mkdir_api open_api link_api symlink_api readlink_api unlink_api remove_api rmdir_api rename_api write_api
	@echo "end clean"

.PHONY: clean
