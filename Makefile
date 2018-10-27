CXXFLAGS += -g
CPPFLAGS += -I/usr/share/verilator/include
CPPFLAGS += -I/usr/share/verilator/include/vltstd
DEBUGGER = gdb

CXXFLAGS += -std=c++11
CXXFLAGS += -Wall -Wextra -Wold-style-cast

LINK.o = $(LINK.cc)

TARGETS = main ctors call

TYPES = Model_device Model_core Avr8

MCU = ATtiny1616

all: $(TARGETS)

$(TARGETS): LDFLAGS += -Llib
$(TARGETS): LDLIBS += -lattinyxc

ctors: CXXFLAGS += -O
call: CXXFLAGS += -O3

call: LDLIBS += -ldl

main.o: model.hh
main.o: CXXFLAGS += -Wno-unused-value # for asserts

call.o: CXXFLAGS += -Wno-cast-function-type # this is the whole point of `call`

run debug: export LD_LIBRARY_PATH=lib
debug: prefix = $(DEBUGGER)
run debug: main
	$(prefix) $(realpath $<)

model.%.xml: model.hh
	castxml -o $@ --castxml-output=1 --castxml-start $* $<

methods.%.txt: model.%.xml
	xmllint --xpath '//Method[@context=string(//Class[@name="$*"]/@id)]/@name' $< | grep -o '"[^"]*"' | tr -d '"' > $@ || (rm $@; false)

methods.%.xi: methods.%.txt
	(echo "#define METHODS_$*_(_) \\"; sed 's/^/    _($*,/; s/$$/) \\/' $<; echo "    // end METHODS_$*_") > $@ || (rm $@; false)

call.o: types.xi
call.o: $(TYPES:%=methods.%.xi)

types.xi:
	($(foreach f,$(TYPES),echo '#include "methods.$f.xi"';) echo '#define TYPE_LIST(_) \'; $(foreach f,$(TYPES),echo '    _($f) \';) echo '    // end TYPE_LIST') > $@ || (rm $@; false)

supposed.%: flatten.h methods.%.xi
	cpp -P -DTYPE=$* $< | tr ' ' '\n' > $@ || (rm $@; false)

run.out: export LD_LIBRARY_PATH=lib
run.out: call
	$(realpath $<) $(MCU) > $@ || (rm $@; false)

actual.%: run.out
	grep $* $< | cut -d: -f4- | cut -d'(' -f1 > $@ || (rm $@; false)

clean:
	$(RM) $(TARGETS) *.o methods.*.txt methods.*.txt methods.*.xi model.*.xml
