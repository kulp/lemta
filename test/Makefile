# Make config
.DEFAULT_GOAL = all

CXXFLAGS += -O3
CXXFLAGS += -std=c++11
CXXFLAGS += -Wall -Wextra -Wold-style-cast
CXXFLAGS += -Werror -Wno-unknown-warning-option
CXXFLAGS += -pedantic-errors

CFLAGS = -Wall -Wextra -Werror -pedantic-errors

LINK.o = $(LINK.cc)

.DELETE_ON_ERROR:

vpath %.hh ../include ../include/internals
vpath %.hh cxx
vpath %.cc cxx
vpath %.lua lua

# Tool config
LUAJIT = luajit
PACKS_URL_BASE = http://packs.download.atmel.com/

# Test config
TYPES = Model Core
LUA_TESTS = $(wildcard cases/*.lua)

# Target config
IMPL = libimpl.so
TARGETS = call $(IMPL)

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

FILES_distclean += packs.mk
include packs.mk
include mk/generate.mk
include $(wildcard mk/*-overrides.mk)

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

check-%-call: check-%-call-rhs.txt
	diff --brief $^

FILES_clean += check-*-call-rhs.txt
check-%-call-rhs.txt: call
	$(realpath $<) "$(TYPE)" "$(LIB)" "$(MCU)" | sed --quiet '/.*::/{s///;s/(.*//;p}' > $@

check-%.lua mk-gen/vars-%.mk: export LUA_PATH := $(LUA_PATH);;lua/?.lua;../lua/?.lua

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

%.elf: AS  = avr-gcc
%.elf: CC  = avr-gcc
%.elf: CXX = avr-g++

MCUFLAGS = $(if $(MCU),-mmcu=$$(echo $(MCU) | tr '[[:upper:]]' '[[:lower:]]'))
%.elf: CFLAGS  += $(MCUFLAGS)
%.elf: ASFLAGS += $(MCUFLAGS)
%.elf: LDFLAGS += $(MCUFLAGS)

vpath %.S cases/src
vpath %.c cases/src

rw.%: MCU = ATtiny84A
sum.%: MCU =# none
io.%: MCU =# none
break.%: MCU =# none

%.elf: %.o
	$(LINK.o) -o $@ $^

FILES_clean += *.hex
FLASH_SECTIONS = text data vectors
%.hex: %.elf
	avr-objcopy $(FLASH_SECTIONS:%=-j .%) -O ihex $< $@

FILES_clean += mk-gen/vars-*.mk
clean:
	$(RM) $(TARGETS) *.o $(FILES_clean)
	-rmdir mk-gen/

distclean: clean
	$(RM) -r $(FILES_distclean)
