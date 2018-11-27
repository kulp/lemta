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

all: $(TARGETS) libimpl-Model.so libimpl-Core.so

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

%.strings.cc: %
	(echo "extern "'"C"'" const char * get_`echo "$*" | tr -c [:alnum:]_ _`(){ return" ; sed 's/^/"/;s/$$/\\n"/' $< ; echo ';}') > $@

FILES_clean += libimpl-*.so
libimpl-%.so: CXXFLAGS += -pedantic -Wall -O3
libimpl-%.so: LDFLAGS += -shared
libimpl-%.so: impl.%.cc header.%.h.strings.cc methods.%.txt.strings.cc | interface.hh
	$(LINK.cc) -include $| -o $@ $^

check-%-z: export LD_LIBRARY_PATH=unpacked/$(LIB_DIR)
check-%-z: check-%-z-rhs.txt
	diff -q $^

FILES_clean += check-*-z-rhs.txt
check-%-z-rhs.txt: call
	$(realpath $<) "$(TYPE)" "$(LIB)" "$(MCU)" | sed -n '/.*::/{s///;s/(.*//;p}' > $@

LUA = luajit
LUA_TESTS = mem.lua regs.lua

define newline


endef

define check_lib_1

check: check-$1
check-$1: LIB_STEM = $1

endef

define check_lib_2

check-$1: check-$1-$2
check-$1-$2: LIB_STEM = $1
check-$1-$2: MCU = $2

$(foreach L,$(LUA_TESTS),$(call check_lib_lua,$1,$2,$L))

endef

define check_lib_lua

check-$1-$2: $(LUA_TESTS:%=check-$1-$2-%)
check-$1-$2-$3: export LD_LIBRARY_PATH=.:unpacked/$(LIB_DIR)
check-$1-$2-$3: export LUA_PATH=;;../lua/?.lua
check-$1-$2-$3: libimpl-Model.so libimpl-Core.so
check-$1-$2-$3: $3
	$(LUA) $3 $1 $2

endef

define check_lib_3

check-$1-$2: check-$1-$2-$3-z
check-$1-$2-$3-z: LIB_STEM = $1
check-$1-$2-$3-z: MCU = $2
check-$1-$2-$3-z: TYPE = $3
check-$1-$2-$3-z: methods.$3.txt

endef

FILES_clean += check-lib-*.mk
ifneq ($(MAKECMDGOALS),clean)
include $(LIB_LIST:%=check-lib-%.mk)
endif

$(LIB_LIST:%=check-lib-%.mk): check-lib-%.mk:
	$(file >$@,$(call check_lib_1,$*) $(foreach m,$(call MCU_NAMES,$*),$(call check_lib_2,$*,$m) $(foreach t,$(TYPES),$(call check_lib_3,$*,$m,$t))))

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
