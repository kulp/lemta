# Make config
.DEFAULT_GOAL = all

# enable warnings as soon as possible, in the preprocessor (applies to both C++
# and C compilation0
CPPFLAGS += -Wall -Wextra -Werror -pedantic-errors

CXXFLAGS += -O3
CXXFLAGS += -std=c++98
CXXFLAGS += -Wold-style-cast -Wno-unknown-warning-option
CXXFLAGS += -Wc++11-compat -Wc++14-compat

# by default we link using the C++ linker
LINK.o = $(LINK.cc)

.DELETE_ON_ERROR:

vpath %.hh ../include ../include/internals
vpath %.hh cxx
vpath %.cc cxx
vpath %.lua lua

# Tool config
LUAJIT = luajit
PACKS_URL_BASE = http://packs.download.atmel.com/
CURL = curl

# Test config
TYPES = Model Core
LUA_TESTS = $(wildcard cases/*.lua)

# Target config
IMPL = libimpl.so
TARGETS = call $(IMPL)
FILES_clean += $(TARGETS)

UNPACKED = unpacked
PACKED = packs
LIB_DIR = simulator/linux64
LIB_STEM = $(error LIB_STEM must be set)
LIB_LIST = $(patsubst lib%.so,%,$(notdir $(wildcard $(UNPACKED)/$(LIB_DIR)/libat*.so)))
LIB = $(UNPACKED)/$(LIB_DIR)/lib$(LIB_STEM).so

QUOTED_STRINGS = sed 's/[^"]*"\([^"]*\)"/\1\n/g'

XMLLINT = xmllint
ifeq ($(shell which $(XMLLINT) 2> /dev/null),)
$(error Need xmllint -- set XMLLINT (currently '$(XMLLINT)'))
endif

FILES_distclean += packs.mk
include packs.mk
include mk/generate.mk
include $(wildcard mk/*-overrides.mk)
include mk/avr.mk

all: $(TARGETS)

call: LDLIBS += -ldl
call.o: CXXFLAGS += -std=c++11
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
	$(XMLLINT) --xpath '//Method[@context=string(//Class[@name="$*"]/@id) and @access="public"]/@name' $< | $(QUOTED_STRINGS) > $@

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
	(echo "extern "'"C"'" const char * get_`echo "$*" | tr --complement [:alnum:]_ _`(){ return" '""' ; sed 's/"/\\"/g; s/.*/"&\\n"/' $< ; echo ';}') > $@

%.so: CXXFLAGS += -fPIC
%.so: LDFLAGS += -shared
$(IMPL): CPPFLAGS += -I../include
$(IMPL): interface.hh shim.cc impl.interface.hh.cc header.interface.hh.h.strings.cc $(TYPES:%=methods.%.txt.strings.cc)
	$(LINK.cc) -include $(filter %.hh,$^) -o $@ $(filter-out %.hh,$^)

check-%-call: check-%-call-rhs.txt
	diff --brief $^

FILES_clean += check-*-call-rhs.txt
check-%-call-rhs.txt: call
	$(realpath $<) "$(TYPE)" "$(LIB)" "$(MCU)" | sed --quiet '/.*::/{s///;s/(.*//;p}' > $@

check-%.lua mk-gen/vars-%.mk: export LUA_PATH := $(LUA_PATH);;lua/?.lua;../lua/?.lua

FILES_distclean += $(PACKED)
$(PACKED)/%.atpack: | $(PACKED)
	$(CURL) --continue-at - --output $@ $(PACKS_URL_BASE)$(@F)

.PRECIOUS: $(PACKS:%=$(PACKED)/%)
.SECONDARY: $(PACKS:%=$(PACKED)/%)
$(PACKED) $(UNPACKED) mk-gen:
	-mkdir $@
unpack: $(PACKS:%.atpack=unpack-%) | $(UNPACKED)
FILES_distclean += $(UNPACKED)/
unpack-%: $(PACKED)/%.atpack | $(UNPACKED)
	unzip -qq -n -d $| $< '$(LIB_DIR)/*.so' '*.pdsc' '*.atdf'

FILES_clean += mk-gen/vars-*.mk
clean:
	$(RM) *.o $(FILES_clean)
	-rmdir mk-gen

distclean: clean
	$(RM) -r $(FILES_distclean)
