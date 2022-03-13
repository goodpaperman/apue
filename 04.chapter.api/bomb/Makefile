all: foo

foo: bar
	touch foo

bar: bar.c
	touch bar 
	sleep 1
	touch bar.c

clean: 
	@echo "start clean..."
	-rm -f foo bar
	@echo "end clean"

.PHONY: clean
