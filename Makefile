CXXFLAGS += -g
CPPFLAGS += -I/usr/share/verilator/include
CPPFLAGS += -I/usr/share/verilator/include/vltstd

CXXFLAGS += -std=c++11
CXXFLAGS += -Wall -Wextra -Wold-style-cast

LINK.o = $(LINK.cc)

TARGETS = main call
LINKED_TARGETS = main

TYPES = Model Core

MCU =# blank means default, can be set specifically

LIB_DIR = lib
LIB_STEM = attinyxc
LIB = $(LIB_DIR)/lib$(LIB_STEM).so

@STDOUT = $@ || (rm $@; false)

all: $(TARGETS)

$(LINKED_TARGETS): LDFLAGS += -L$(LIB_DIR)
$(LINKED_TARGETS): LDLIBS += -l$(LIB_STEM)

call: CXXFLAGS += -O3

call: LDLIBS += -ldl

main.o: model.hh
main.o: CXXFLAGS += -Wno-unused-value # for asserts

call.o: CXXFLAGS += -Wno-cast-function-type # this is the whole point of `call`

model.%.xml: model.hh
	castxml -o $@ --castxml-output=1 --castxml-start $* $<

methods.%.txt: model.%.xml
	xmllint --xpath '//Method[@context=string(//Class[@name="$*"]/@id) and @access="public"]/@name' $< | grep -o '"[^"]*"' | tr -d '"' > $(@STDOUT)

methods.%.xi: methods.%.txt
	(echo "#define METHODS_$*_(_) \\"; sed 's/^/    _($*,/; s/$$/) \\/' $<; echo "    // end METHODS_$*_") > $(@STDOUT)

call.o: types.xi
call.o: $(TYPES:%=methods.%.xi)

types.xi:
	($(foreach f,$(TYPES),echo '#include "methods.$f.xi"';) echo '#define TYPE_LIST(_) '\\; $(foreach f,$(TYPES),echo '    _($f) '\\;) echo '    // end TYPE_LIST') > $(@STDOUT)

supposed.%: flatten.h methods.%.xi
	cpp -P -DTYPE=$* $< | tr ' ' '\n' > $(@STDOUT)

actual.%: export LD_LIBRARY_PATH=$(LIB_DIR)
actual.%: call
	$(realpath $<) "$*" "$(LIB)" "$(MCU)" | cut -d: -f4- | cut -s -d'(' -f1 | grep -v '^$$' > $(@STDOUT)

check.%: supposed.% actual.%
	diff -q $^

check: $(TYPES:%=check.%)
check: ;

clean:
	$(RM) $(TARGETS) *.o methods.*.txt methods.*.txt methods.*.xi model.*.xml types.xi
