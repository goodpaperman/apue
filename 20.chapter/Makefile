all: db_demo db dbh

dbh: dbh.o apue.o apue_dbh.o
	gcc -Wall -g $^ -o $@

dbh.o: db.c apue_db.h ../apue.h
	gcc -Wall -g -c $< -o $@ -DHAS_HASHSIZE

db: db.o apue.o apue_db.o
	gcc -Wall -g $^ -o $@

db.o: db.c apue_db.h ../apue.h
	gcc -Wall -g -c $< -o $@ 

db_demo: db_demo.o apue.o apue_db.o
	gcc -Wall -g $^ -o $@

db_demo.o: db_demo.c apue_db.h ../apue.h
	gcc -Wall -g -c $< -o $@ 

apue_dbh.o: apue_db.c apue_db.h
	gcc -Wall -g -c $< -o $@ -DHAS_HASHSIZE

apue_db.o: apue_db.c apue_db.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp db_demo db dbh
	@echo "end clean"

.PHONY: clean
