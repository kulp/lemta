.PHONY: all test clean
all:
	$(MAKE) -C test $@
test:
	$(MAKE) -C test check
clean:
	# no action

