SHELL = /bin/bash # we use bash-isms

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

LIB_DIR = unpacked/simulator/linux64
LIB_STEM = $(error LIB_STEM must be set)
LIB_LIST = $(patsubst lib%.so,%,$(notdir $(wildcard unpacked/simulator/linux64/libat*.so)))
LIB = $(LIB_DIR)/lib$(LIB_STEM).so

vpath %.hh ../include ../include/internals
CPPFLAGS += -I../include

@STDOUT = $@ || (rm $@; false)

all: $(TARGETS)

$(LINKED_TARGETS): LDFLAGS += -L$(LIB_DIR)
$(LINKED_TARGETS): LDLIBS += -l$(LIB_STEM)

call: CXXFLAGS += -O3

call: LDLIBS += -ldl

main.o: CPPFLAGS += -I../include/internals
main.o: model.hh interface.hh
main.o: CXXFLAGS += -Wno-unused-value # for asserts

call.o: interface.hh
call.o: CXXFLAGS += -Wno-cast-function-type # this is the whole point of `call`

model.%.xml: interface.hh
	castxml -o $@ --castxml-output=1 --castxml-start $* $<

methods.%.txt: model.%.xml
	xmllint --xpath '//Method[@context=string(//Class[@name="$*"]/@id) and @access="public"]/@name' $< | grep -o '"[^"]*"' | tr -d '"' > $(@STDOUT)

methods.%.xi: methods.%.txt
	(echo "#define METHODS_$*_(_) \\"; sed 's/^/    _($*,/; s/$$/) \\/' $<; echo "    // end METHODS_$*_") > $(@STDOUT)

call.o: types.xi
call.o: $(TYPES:%=methods.%.xi)

types.xi:
	($(foreach f,$(TYPES),echo '#include "methods.$f.xi"';) echo '#define TYPE_LIST(_) '\\; $(foreach f,$(TYPES),echo '    _($f) '\\;) echo '    // end TYPE_LIST') > $(@STDOUT)

define check_lib
check-$1-$2: LIB_STEM = $1
check-$1-$2: export LD_LIBRARY_PATH=$(LIB_DIR)
check-$1-$2: flatten.h call methods.$2.xi
	diff -q \
	    <(cpp -P -DTYPE=$2 $$< | tr ' ' '\n') \
	    <($$(realpath $$(word 2,$$^)) "$2" "$$(LIB)" "$$(MCU)" | cut -d: -f4- | cut -s -d'(' -f1 | grep -v '^$$$$')
endef

$(foreach l,$(LIB_LIST),$(foreach t,$(TYPES),$(eval $(call check_lib,$l,$t))))
CHECK_TARGETS = $(foreach l,$(LIB_LIST),$(foreach t,$(TYPES),check-$l-$t))

check: $(CHECK_TARGETS)
check: ;

PACKS_URL_BASE = http://packs.download.atmel.com/
packs:
	mkdir -p $@
get-latest-packs: | packs
	cd $| && \
	    curl --silent '$(PACKS_URL_BASE)' | \
	    xmllint --html --xpath //button/@data-link - | \
	    grep -o '"[^"]*"' | tr -d '"' | \
	    grep -e XMEGA -e ATautomotive -e ATmega -e ATtiny | \
	    sort --reverse --field-separator=. --key=1,2 --unique | \
	    sed 's#^#$(PACKS_URL_BASE)#' | \
	    xargs -n1 curl --remote-name --continue-at -

unpacked:
	mkdir -p $@
unpack: | unpacked
	for f in packs/*.atpack ; do unzip -qq -n -d "$|" "$$f" '*/linux64/*.so' ; done

clean:
	$(RM) $(TARGETS) *.o methods.*.txt methods.*.txt methods.*.xi model.*.xml types.xi
