.PHONY: all test clean
all:
	$(MAKE) -C test $@
test:
	$(MAKE) -C test check run-call
clean:
	# no action

