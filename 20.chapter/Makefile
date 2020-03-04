all: db_demo

db_demo: db_demo.o apue.o apue_db.o
	gcc -Wall -g $^ -o $@

db_demo.o: db_demo.c apue_db.h ../apue.h
	gcc -Wall -g -c $< -o $@ 

apue_db.o: apue_db.c apue_db.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp db_demo
	@echo "end clean"

.PHONY: clean
