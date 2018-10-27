CXXFLAGS += -g
CPPFLAGS += -I/usr/share/verilator/include
CPPFLAGS += -I/usr/share/verilator/include/vltstd
DEBUGGER = gdb

CXXFLAGS += -std=c++11
CXXFLAGS += -Wall -Wextra -Wold-style-cast

LINK.o = $(LINK.cc)

TARGETS = main ctors call

all: $(TARGETS)

$(TARGETS): LDFLAGS += -Llib
$(TARGETS): LDLIBS += -lattinyxc

ctors: CXXFLAGS += -O
#call: CXXFLAGS += -O3

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

call.o: methods.Model_device.xi
call.o: methods.Model_core.xi

clean:
	$(RM) $(TARGETS) *.o methods.*.txt methods.*.txt methods.*.xi model.*.xml
