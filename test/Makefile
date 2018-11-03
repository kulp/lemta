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

LIB_DIR = simulator/linux64
LIB_STEM = $(error LIB_STEM must be set)
LIB_LIST = $(patsubst lib%.so,%,$(notdir $(wildcard unpacked/$(LIB_DIR)/libat*.so)))
LIB = unpacked/$(LIB_DIR)/lib$(LIB_STEM).so

MCU_NAMES = $(shell xmllint unpacked/*.pdsc --xpath '//*[local-name()="property"][@name="com.atmel.avrdbg.tool.simulator.model.linux64"][@value="$(LIB_DIR)/lib$1.so"]/ancestor::device//*[local-name()="property"][@name="com.atmel.avrdbg.tool.simulator.key"]/@value' 2> /dev/null | grep -o '"[^"]*"' | tr -d '"')

vpath %.hh ../include ../include/internals
CPPFLAGS += -I../include

@STDOUT = $@ || (rm $@; false)

all: $(TARGETS)

$(LINKED_TARGETS): LDFLAGS += -Lunpacked/$(LIB_DIR)
$(LINKED_TARGETS): LDLIBS += -l$(LIB_STEM)

main.o: CPPFLAGS += -I../include/internals
main.o: model.hh interface.hh
main.o: CXXFLAGS += -Wno-unused-value # for asserts

call: CXXFLAGS += -O3
call: LDLIBS += -ldl
call.o: interface.hh
call.o: CXXFLAGS += -Wno-cast-function-type # this is the whole point of `call`
call.o: types.xi
call.o: $(TYPES:%=methods.%.xi)

model.%.xml: interface.hh
	castxml -o $@ --castxml-output=1 --castxml-start $* $<

methods.%.txt: model.%.xml
	xmllint --xpath '//Method[@context=string(//Class[@name="$*"]/@id) and @access="public"]/@name' $< | grep -o '"[^"]*"' | tr -d '"' > $(@STDOUT)

methods.%.xi: methods.%.txt
	(echo "#define METHODS_$*_(_) \\"; sed 's/^/    _($*,/; s/$$/) \\/' $<; echo "    // end METHODS_$*_") > $(@STDOUT)

types.xi:
	($(foreach f,$(TYPES),echo '#include "methods.$f.xi"';) echo '#define TYPE_LIST(_) '\\; $(foreach f,$(TYPES),echo '    _($f) '\\;) echo '    // end TYPE_LIST') > $(@STDOUT)

check-%: export LD_LIBRARY_PATH=unpacked/$(LIB_DIR)
define check_lib

CHECK_TARGETS += check-$1-$2-$3
check-$1-$2-$3: LIB_STEM = $1
check-$1-$2-$3: MCU = $2
check-$1-$2-$3: TYPE = $3
check-$1-$2-$3: flatten.h call methods.$3.xi
	diff -q \
	    <(cpp -P -DTYPE=$$(TYPE) $$< | tr ' ' '\n') \
	    <($$(realpath $$(word 2,$$^)) "$$(TYPE)" "$$(LIB)" "$$(MCU)" | cut -d: -f4- | cut -s -d'(' -f1 | grep -v '^$$$$')

endef

ifneq ($(MAKECMDGOALS),clean)
include $(LIB_LIST:%=check-lib-%.mk)
endif

$(LIB_LIST:%=check-lib-%.mk): check-lib-%.mk:
	$(file >$@,$(foreach m,$(call MCU_NAMES,$*),$(foreach t,$(TYPES),$(call check_lib,$*,$m,$t))))

check: $(CHECK_TARGETS)
ifeq ($(CHECK_TARGETS),) # hackily recursively make ourselves to bring in includes
	@$(MAKE) $@
else
# empty target to suppress inferred rules
	@true
endif

PACKS_URL_BASE = http://packs.download.atmel.com/
packs:
	mkdir -p $@
PACK_TYPES = ATautomotive ATmega ATtiny XMEGA
get-latest-packs: | packs
	cd $| && \
	    curl --silent '$(PACKS_URL_BASE)' | \
	    xmllint --html --xpath //button/@data-link - | \
	    grep -o '"[^"]*"' | tr -d '"' | \
	    grep $(PACK_TYPES:%=-e %) | \
	    sort --reverse --field-separator=. --key=1,2 --unique | \
	    sed 's#^#$(PACKS_URL_BASE)#' | \
	    xargs -n1 curl --remote-name --continue-at -

unpacked:
	mkdir -p $@
unpack: | unpacked
	for f in packs/*.atpack ; do unzip -qq -n -d "$|" "$$f" '$(LIB_DIR)/*.so' '*.pdsc' ; done

clean:
	$(RM) $(TARGETS) *.o methods.*.txt methods.*.xi model.*.xml types.xi check-lib-*.mk
