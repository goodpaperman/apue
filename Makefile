all: 
	$(MAKE) -C 01.chapter 
	$(MAKE) -C 02.chapter 
	$(MAKE) -C 03.chapter 
	$(MAKE) -C 04.chapter 
	$(MAKE) -C 05.chapter 
	$(MAKE) -C 06.chapter 
	$(MAKE) -C 07.chapter 
	$(MAKE) -C 08.chapter 
	$(MAKE) -C 09.chapter 
	$(MAKE) -C 10.chapter 
	$(MAKE) -C 11.chapter 
	$(MAKE) -C 12.chapter 
	$(MAKE) -C 13.chapter 


clean:
	$(MAKE) -C 01.chapter clean 
	$(MAKE) -C 02.chapter clean 
	$(MAKE) -C 03.chapter clean 
	$(MAKE) -C 04.chapter clean 
	$(MAKE) -C 05.chapter clean 
	$(MAKE) -C 06.chapter clean 
	$(MAKE) -C 07.chapter clean 
	$(MAKE) -C 08.chapter clean 
	$(MAKE) -C 09.chapter clean 
	$(MAKE) -C 10.chapter clean 
	$(MAKE) -C 11.chapter clean 
	$(MAKE) -C 12.chapter clean 
	$(MAKE) -C 13.chapter clean 

.PHONY: clean
