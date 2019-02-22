.PHONY: all test clean
all:
	$(MAKE) -C test $@
test:
	$(MAKE) -C test check vars
clean:
	# no action

