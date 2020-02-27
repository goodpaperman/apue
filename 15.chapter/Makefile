all: pipestat pipemulti pipeatom pipepoll pipepage ppage pmulti tolower ptolower add2 add3 padd2 ffread ffwrite ffrw xsiget xsique xsisem xsishm addr zroshm pipeselect msgid shmlst

shmlst: shmlst.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

shmlst.o: shmlst.c ../apue.h
	gcc -Wall -g -c $< -o $@

msgid: msgid.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

msgid.o: msgid.c ../apue.h
	gcc -Wall -g -c $< -o $@

pipeselect: pipeselect.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

pipeselect.o: pipeselect.c ../apue.h
	gcc -Wall -g -c $< -o $@

zroshm: zroshm.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

zroshm.o: zroshm.c ../apue.h
	gcc -Wall -g -c $< -o $@ -D_BSD_SOURCE

addr: addr.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

addr.o: addr.c ../apue.h
	gcc -Wall -g -c $< -o $@

xsishm: xsishm.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

xsishm.o: xsishm.c ../apue.h
	gcc -Wall -g -c $< -o $@

xsisem: xsisem.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

xsisem.o: xsisem.c ../apue.h
	gcc -Wall -g -c $< -o $@

xsique: xsique.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

xsique.o: xsique.c ../apue.h
	gcc -Wall -g -c $< -o $@

xsiget: xsiget.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

xsiget.o: xsiget.c ../apue.h
	gcc -Wall -g -c $< -o $@

ffrw: ffrw.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

ffrw.o: ffrw.c ../apue.h
	gcc -Wall -g -c $< -o $@

ffwrite: ffwrite.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

ffwrite.o: ffwrite.c ../apue.h
	gcc -Wall -g -c $< -o $@

ffread: ffread.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

ffread.o: ffread.c ../apue.h
	gcc -Wall -g -c $< -o $@

padd2: padd2.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

padd2.o: padd2.c ../apue.h
	gcc -Wall -g -c $< -o $@

add3: add3.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

add3.o: add3.c ../apue.h
	gcc -Wall -g -c $< -o $@ -DSETVBUF

add2: add2.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

add2.o: add2.c ../apue.h
	gcc -Wall -g -c $< -o $@

ptolower: ptolower.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

ptolower.o: ptolower.c ../apue.h
	gcc -Wall -g -c $< -o $@

tolower: tolower.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

tolower.o: tolower.c ../apue.h
	gcc -Wall -g -c $< -o $@

pmulti: pmulti.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

pmulti.o: pmulti.c ../apue.h
	gcc -Wall -g -c $< -o $@

ppage: ppage.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

ppage.o: ppage.c ../apue.h
	gcc -Wall -g -c $< -o $@

pipepage: pipepage.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

pipepage.o: pipepage.c ../apue.h
	gcc -Wall -g -c $< -o $@

pipepoll: pipepoll.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

pipepoll.o: pipepoll.c ../apue.h
	gcc -Wall -g -c $< -o $@

pipeatom: pipeatom.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

pipeatom.o: pipeatom.c ../apue.h
	gcc -Wall -g -c $< -o $@

pipemulti: pipemulti.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

pipemulti.o: pipemulti.c ../apue.h
	gcc -Wall -g -c $< -o $@

pipestat: pipestat.o apue.o 
	gcc -Wall -g $^ -o $@ -lpthread

pipestat.o: pipestat.c ../apue.h
	gcc -Wall -g -c $< -o $@

log.o: ../log.c ../log.h
	gcc -Wall -g -c $< -o $@

apue.o: ../apue.c ../apue.h 
	gcc -Wall -g -c $< -o $@ -D__USE_BSD -DUSE_PTHREAD

clean: 
	@echo "start clean..."
	-rm -f *.o core.* *.log *~ *.swp pipestat pipemulti pipeatom pipepoll pipepage ppage pmulti tolower ptolower add2 add3 padd2 ffread ffwrite ffrw xsiget xsique xsisem xsishm addr zroshm msgid shmlst
	@echo "end clean"

.PHONY: clean
