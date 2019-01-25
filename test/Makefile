CXXFLAGS += -O3
CXXFLAGS += -std=c++11
CXXFLAGS += -Wall -Wextra -Wold-style-cast
CXXFLAGS += -Werror -Wno-unknown-warning-option
CXXFLAGS += -pedantic-errors

LINK.o = $(LINK.cc)

IMPL = libimpl.so
TARGETS = call $(IMPL)

TYPES = Model Core

UNPACKED = unpacked
LIB_DIR = simulator/linux64
LIB_STEM = $(error LIB_STEM must be set)
LIB_LIST = $(patsubst lib%.so,%,$(notdir $(wildcard $(UNPACKED)/$(LIB_DIR)/libat*.so)))
LIB = $(UNPACKED)/$(LIB_DIR)/lib$(LIB_STEM).so

QUOTED_STRINGS = sed 's/[^"]*"\([^"]*\)"/\1\n/g'

# For now we use the win32 library stem instead of the linux64 library stem
# because the .pdsc files at the time of this writing erroneously fail to
# mention some .so files even though they exist, whereas they mention the .dll
# files that also exist.
MCU_NAMES = $(shell xmllint $(UNPACKED)/*.pdsc --xpath '//*[local-name()="property"][@name="com.atmel.avrdbg.tool.simulator.model.win32"][@value="simulator/win32/lib$1.dll"]/ancestor::device//*[local-name()="property"][@name="com.atmel.avrdbg.tool.simulator.key"]/@value' 2> /dev/null | $(QUOTED_STRINGS))

vpath %.hh ../include ../include/internals
vpath %.hh cxx
vpath %.cc cxx

vpath %.lua lua

LUAJIT = luajit

.DELETE_ON_ERROR:

all: $(TARGETS)

call: LDLIBS += -ldl
call.o: CPPFLAGS += -I../include -I.
call.o: CXXFLAGS += -Wno-cast-function-type # this is the whole point of `call`
call.o: interface.hh dynamic.hh
call.o: types.xi $(TYPES:%=methods.%.xi)

FILES_clean += model.*.xml
model.%.xml: %
	castxml -o $@ --castxml-output=1 $<

FILES_clean += methods.*.txt
.INTERMEDIATE: $(TYPES:%=methods.%.txt)
methods.%.txt: model.interface.hh.xml
	xmllint --xpath '//Method[@context=string(//Class[@name="$*"]/@id) and @access="public"]/@name' $< | $(QUOTED_STRINGS) > $@

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
header.%.h impl.%.cc: gen.lua model.%.xml
	$(LUAJIT) $^ header.$*.h impl.$*.cc

FILES_clean += *.strings.cc
%.strings.cc: %
	(echo "extern "'"C"'" const char * get_`echo "$*" | tr --complement [:alnum:]_ _`(){ return" '""' ; sed 's/.*/"&\\n"/' $< ; echo ';}') > $@

%.so: CXXFLAGS += -fPIC
%.so: LDFLAGS += -shared
$(IMPL): impl.interface.hh.cc header.interface.hh.h.strings.cc $(TYPES:%=methods.%.txt.strings.cc) | interface.hh
	$(LINK.cc) -include $| -o $@ $^

check-%-call: export LD_LIBRARY_PATH=$(UNPACKED)/$(LIB_DIR)
check-%-call: check-%-call-rhs.txt
	diff --brief $^

FILES_clean += check-*-call-rhs.txt
check-%-call-rhs.txt: call
	$(realpath $<) "$(TYPE)" "$(LIB)" "$(MCU)" | sed --quiet '/.*::/{s///;s/(.*//;p}' > $@

LUA_TESTS = $(wildcard cases/*.lua)

check-%.lua mk-gen/vars-%.mk: export LD_LIBRARY_PATH=.:$(UNPACKED)/$(LIB_DIR)
check-%.lua mk-gen/vars-%.mk: export LUA_PATH := $(LUA_PATH);;lua/?.lua;../lua/?.lua

define check_lib_1

check: check-$1
check-$1 check-$1-%: LIB_STEM = $1

$(foreach m,$(call MCU_NAMES,$1),$(call check_lib_2,$1,$m))
endef

define check_lib_2

check-$1: check-$1-$2
check-$1-$2 check-$1-$2-%: MCU = $2

check-$1-$2: $(LUA_TESTS:%=check-$1-$2-%)
$(LUA_TESTS:%=check-$1-$2-%): check-$1-$2-%: % $$(IMPL)
	$$(if $$(SKIP-$$(LIB_STEM)-$$(MCU)-$$<),\
	    $$(warning Target $$@ fails and has been disabled pending investigation),\
	    $$(LUAJIT) $$^ $$(LIB_STEM) $$(MCU))

vars: mk-gen/vars-$1-$2.mk

mk-gen/vars-$1-$2.mk: lua/suss.lua $$(IMPL) | mk-gen
	$$(LUAJIT) $$^ lib$1.so $2 2> /dev/null | grep = > $$@

$(foreach t,$(TYPES),$(call check_lib_3,$1,$2,$t))
endef

define check_lib_3

check-$1-$2: check-$1-$2-$3-call
check-$1-$2-$3: check-$1-$2-$3-call
check-$1-$2-$3-call: TYPE = $3
check-$1-$2-$3-call: methods.$3.txt

endef

FILES_clean += mk-gen/check-lib-*.mk
ifeq ($(findstring clean,$(MAKECMDGOALS)),)
include $(LIB_LIST:%=mk-gen/check-lib-%.mk)
endif

include $(wildcard mk/*-overrides.mk)

# The use of $(file ...) here requires Make version 4+
$(LIB_LIST:%=mk-gen/check-lib-%.mk): mk-gen/check-lib-%.mk: | mk-gen
	$(file >$@,$(call check_lib_1,$*))

PACKS_URL_BASE = http://packs.download.atmel.com/

FILES_distclean += packs.mk
include packs.mk

FILES_distclean += packs/
packs/%.atpack: | packs
	curl --continue-at - --output $@ $(PACKS_URL_BASE)$(@F)

.PRECIOUS: $(PACKS:%=packs/%)
.SECONDARY: $(PACKS:%=packs/%)
packs $(UNPACKED) mk-gen:
	-mkdir $@
unpack: $(PACKS:%.atpack=unpack-%) | $(UNPACKED)
FILES_distclean += $(UNPACKED)/
unpack-%: packs/%.atpack | $(UNPACKED)
	unzip -qq -n -d $| $< '$(LIB_DIR)/*.so' '*.pdsc' '*.atdf'

FILES_clean += mk-gen/vars-*.mk
clean:
	$(RM) $(TARGETS) *.o $(FILES_clean)
	-rmdir mk-gen/

distclean: clean
	$(RM) -r $(FILES_distclean)
