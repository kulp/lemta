CXXFLAGS += -g
CXXFLAGS += -O3
CXXFLAGS += -std=c++11
CXXFLAGS += -Wall -Wextra -Wold-style-cast

LINK.o = $(LINK.cc)

TARGETS = main call

TYPES = Model Core
IMPLS = $(TYPES:%=libimpl-%.so)

LIB_DIR = simulator/linux64
LIB_STEM = $(error LIB_STEM must be set)
LIB_LIST = $(patsubst lib%.so,%,$(notdir $(wildcard unpacked/$(LIB_DIR)/libat*.so)))
LIB = unpacked/$(LIB_DIR)/lib$(LIB_STEM).so

# For now we use the win32 library stem instead of the linux64 library stem
# because the .pdsc files at the time of this writing erroneously fail to
# mention some .so files even though they exist, whereas they mention the .dll
# files that also exist.
MCU_NAMES = $(shell xmllint unpacked/*.pdsc --xpath '//*[local-name()="property"][@name="com.atmel.avrdbg.tool.simulator.model.win32"][@value="simulator/win32/lib$1.dll"]/ancestor::device//*[local-name()="property"][@name="com.atmel.avrdbg.tool.simulator.key"]/@value' 2> /dev/null | grep -o '"[^"]*"' | tr -d '"')

vpath %.hh ../include ../include/internals
vpath %.hh cxx
vpath %.cc cxx

vpath %.lua lua

CPPFLAGS += -I../include
CPPFLAGS += -I$(CURDIR)

LUA = lua
LUAJIT = luajit

.DELETE_ON_ERROR:

all: $(TARGETS) $(IMPLS)

main: LDLIBS += -ldl
main.o: CPPFLAGS += -I../include/internals
main.o: CXXFLAGS += -Wno-unused-value # for asserts
main.o: model.hh interface.hh dynamic.hh

call: LDLIBS += -ldl
call.o: CXXFLAGS += -Wno-cast-function-type # this is the whole point of `call`
call.o: interface.hh dynamic.hh
call.o: types.xi $(TYPES:%=methods.%.xi)

# dummy rule
run-call: ;

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
types.xi: open=(
types.xi: close=)
types.xi:
	($(TYPES:%=echo '#include "methods.%.xi"';) ) > $@
	echo '#define TYPE_LIST(_)' $(TYPES:%= '_$(open)%$(close)') >> $@

FILES_clean += header.*.h impl.*.cc
# If xmlua can be installed for plain Lua, then the following rule should call
# $(LUA) instead of $(LUAJIT)
header.%.h impl.%.cc: gen.lua model.%.xml
	$(LUAJIT) $^ header.$*.h impl.$*.cc

%.strings.cc: %
	(echo "extern "'"C"'" const char * get_`echo "$*" | tr -c [:alnum:]_ _`(){ return" ; sed 's/^/"/;s/$$/\\n"/' $< ; echo ';}') > $@

FILES_clean += libimpl-*.so
libimpl-%.so: CXXFLAGS += -pedantic -Wall -O3 -fPIC
libimpl-%.so: LDFLAGS += -shared
libimpl-%.so: impl.%.cc header.%.h.strings.cc methods.%.txt.strings.cc | interface.hh
	$(LINK.cc) -include $| -o $@ $^

run-call-% check-%-z: export LD_LIBRARY_PATH=unpacked/$(LIB_DIR)
check-%-z: check-%-z-rhs.txt
	diff -q $^

FILES_clean += check-*-z-rhs.txt
check-%-z-rhs.txt: call
	$(realpath $<) "$(TYPE)" "$(LIB)" "$(MCU)" | sed -n '/.*::/{s///;s/(.*//;p}' > $@

LUA_TESTS = $(wildcard cases/*.lua)

check-%.lua: export LD_LIBRARY_PATH=.:unpacked/$(LIB_DIR)
check-%.lua: export LUA_PATH := $(LUA_PATH);;../lua/?.lua

define check_lib_1

check: check-$1
check-$1 check-$1-%: LIB_STEM = $1

run-call: run-call-$1

$(foreach m,$(call MCU_NAMES,$1),$(call check_lib_2,$1,$m))
endef

define check_lib_2

check-$1: check-$1-$2
check-$1-$2 check-$1-$2-%: MCU = $2

run-call-$1: run-call-$1-$2

check-$1-$2: $(LUA_TESTS:%=check-$1-$2-%)
$(LUA_TESTS:%=check-$1-$2-%): check-$1-$2-%: % $(IMPLS)
	$$(if $$(SKIP-$$(LIB_STEM)-$$(MCU)-$$<),\
	    $$(warning Target $$@ fails and has been disabled pending investigation),\
	    $$(LUAJIT) $$< $$(LIB_STEM) $$(MCU))

$(foreach t,$(TYPES),$(call check_lib_3,$1,$2,$t))
endef

define check_lib_3

check-$1-$2: check-$1-$2-$3-z
check-$1-$2-$3: check-$1-$2-$3-z
check-$1-$2-$3-z: TYPE = $3
check-$1-$2-$3-z: methods.$3.txt

run-call-$1-$2: run-call-$1-$2-$3

run-call-$1-$2-$3: call
	$$(realpath $$<) $3 lib$1.so $2

endef

FILES_clean += check-lib-*.mk
ifneq ($(MAKECMDGOALS),clean)
include $(LIB_LIST:%=check-lib-%.mk)
endif

include $(wildcard *-overrides.mk)

# The use of $(file ...) here requires Make version 4+
$(LIB_LIST:%=check-lib-%.mk): check-lib-%.mk:
	$(file >$@,$(call check_lib_1,$*))

PACKS_URL_BASE = http://packs.download.atmel.com/
PACK_TYPES = ATautomotive ATmega ATtiny XMEGA
get-pack-list: packs/index.html

FILES_clean += packs.mk
ifeq ($(wildcard packs/index.html),)
packs.mk: | packs
	ls -1 $|/ | sed 's/^/PACKS += /' > $@
else
packs.mk: packs/index.html
	xmllint --html --xpath //button/@data-link $< | \
	    grep -o '"[^"]*"' | tr -d '"' | \
	    grep $(PACK_TYPES:%=-e %) | \
	    sort --reverse --field-separator=. --key=1,2 --unique | \
	    sed 's/^/PACKS += /' \
	    > $@
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
