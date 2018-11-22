VERILATOR_CFLAGS := $(shell pkg-config --cflags verilator)

CXXFLAGS += -g
CXXFLAGS += -O3
CXXFLAGS += -std=c++11
CXXFLAGS += -Wall -Wextra -Wold-style-cast

CPPFLAGS += $(VERILATOR_CFLAGS)

LINK.o = $(LINK.cc)

TARGETS = main call

TYPES = Model Core

LIB_DIR = simulator/linux64
LIB_STEM = $(error LIB_STEM must be set)
LIB_LIST = $(patsubst lib%.so,%,$(notdir $(wildcard unpacked/$(LIB_DIR)/libat*.so)))
LIB = unpacked/$(LIB_DIR)/lib$(LIB_STEM).so

MCU_NAMES = $(shell xmllint unpacked/*.pdsc --xpath '//*[local-name()="property"][@name="com.atmel.avrdbg.tool.simulator.model.linux64"][@value="$(LIB_DIR)/lib$1.so"]/ancestor::device//*[local-name()="property"][@name="com.atmel.avrdbg.tool.simulator.key"]/@value' 2> /dev/null | grep -o '"[^"]*"' | tr -d '"')

vpath %.hh ../include ../include/internals
CPPFLAGS += -I../include

.DELETE_ON_ERROR:

all: $(TARGETS)

main: LDLIBS += -ldl
main.o: CPPFLAGS += -I../include/internals
main.o: CXXFLAGS += -Wno-unused-value # for asserts
main.o: model.hh interface.hh dynamic.hh

call: LDLIBS += -ldl
call.o: CXXFLAGS += -Wno-cast-function-type # this is the whole point of `call`
call.o: interface.hh dynamic.hh
call.o: types.xi $(TYPES:%=methods.%.xi)

FILES_clean += model.*.xml
model.%.xml: interface.hh
	castxml -o $@ --castxml-output=1 --castxml-start $* $<

FILES_clean += methods.*.txt
methods.%.txt: model.%.xml
	xmllint --xpath '//Method[@context=string(//Class[@name="$*"]/@id) and @access="public"]/@name' $< | grep -o '"[^"]*"' | tr -d '"' > $@

FILES_clean += methods.*.xi
methods.%.xi: methods.%.txt
	(echo "#define METHODS_$*_(_) \\"; sed 's/^/    _($*,/; s/$$/) \\/' $<; echo "    // end METHODS_$*_") > $@

FILES_clean += types.xi
types.xi:
	($(foreach f,$(TYPES),echo '#include "methods.$f.xi"';) echo '#define TYPE_LIST(_) '\\; $(foreach f,$(TYPES),echo '    _($f) '\\;) echo '    // end TYPE_LIST') > $@

header.%.h impl.%.cc: gen.lua model.%.xml
	luajit $^ header.$*.h impl.$*.cc

FILES_clean += libimpl.*.so
libimpl.%.so: CXXFLAGS += -pedantic -Wall -O3
libimpl.%.so: LDFLAGS += -shared
libimpl.%.so: impl.%.cc interface.hh
	$(LINK.cc) -include $(word 2,$^) -o $@ $<

check-%: export LD_LIBRARY_PATH=unpacked/$(LIB_DIR)
check-%: check-%-rhs.txt
	diff -q $^

FILES_clean += check-*-rhs.txt
check-%-rhs.txt: call
	$(realpath $<) "$(TYPE)" "$(LIB)" "$(MCU)" | sed -n '/.*::/{s///;s/(.*//;p}' > $@

define check_lib

CHECK_TARGETS += check-$1-$2-$3
check-$1-$2-$3: LIB_STEM = $1
check-$1-$2-$3: MCU = $2
check-$1-$2-$3: TYPE = $3
check-$1-$2-$3: methods.$3.txt

endef

FILES_clean += check-lib-*.mk
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
PACK_TYPES = ATautomotive ATmega ATtiny XMEGA
get-pack-list: packs/index.html

FILES_clean += packs.mk
ifeq ($(wildcard packs/index.html),)
packs.mk: $(wildcard packs/*.atpack)
	(echo define PACKS ; ls -1 $^ ; echo endef) > $@
else
packs.mk: packs/index.html
	(echo define PACKS ; \
	    xmllint --html --xpath //button/@data-link $< | \
	    grep -o '"[^"]*"' | tr -d '"' | \
	    grep $(PACK_TYPES:%=-e %) | \
	    sort --reverse --field-separator=. --key=1,2 --unique ; \
	echo endef) > packs.mk
endif

include packs.mk

get-latest-packs: $(PACKS:%=packs/%)

packs:
	mkdir -p $@
packs/index.html: | packs
	curl -o $@ '$(PACKS_URL_BASE)'
packs/%.atpack: | packs
	curl --continue-at - --output $@ $(PACKS_URL_BASE)$(@F)

unpacked:
	mkdir -p $@
unpack: $(PACKS:%.atpack=unpack-%) | unpacked
unpack-%: packs/%.atpack | unpacked
	unzip -qq -n -d $| $< '$(LIB_DIR)/*.so' '*.pdsc' '*.atdf'

clean:
	$(RM) $(TARGETS) *.o $(FILES_clean)

distclean: clean
	$(RM) -r packs unpacked
