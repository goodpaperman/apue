all: foo

foo: bar
	touch foo

bar: bar.c
	touch bar 
	sleep 0.01  # make bar.c mtime newer than bar to trigger make next time...
	touch bar.c
	sleep 0.01  # restore bar.c mtime
	touch -r bar bar.c

clean: 
	@echo "start clean..."
	-rm -f foo bar
	@echo "end clean"

.PHONY: clean
