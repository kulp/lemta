# For now we use the win32 library stem instead of the linux64 library stem
# because the .pdsc files at the time of this writing erroneously fail to
# mention some .so files even though they exist, whereas they mention the .dll
# files that also exist.
FILES_clean += mk-gen/mcus-*.mk
mk-gen/mcus-%.mk: $(UNPACKED)/*.pdsc | mk-gen
	@grep --files-with-matches "$*" $^ | xargs $(XMLLINT) --xpath '//*[local-name()="property"][@name="com.atmel.avrdbg.tool.simulator.model.win32"][@value="simulator/win32/lib$*.dll"]/ancestor::device//*[local-name()="property"][@name="com.atmel.avrdbg.tool.simulator.key"]/@value' | grep --only-matching '"[^"]*"' | sed 's/^/MCU_NAMES_$* += /; s/"//g' > $@

define check_lib_1

check: check-$1
check-$1 check-$1-%: LIB_STEM = $1
mk-gen/vars-$1-%.mk: LIB_STEM = $1

$(foreach m,$(MCU_NAMES_$1),$(call check_lib_2,$1,$m))
endef

define check_lib_2

check-$1: check-$1-$2
check-$1-$2 check-$1-$2-%: MCU = $2

MCU_LIST += $1-$2
check-$1-$2: $$(LUA_TESTS:%=check-$1-$2-%)
$$(LUA_TESTS:%=check-$1-$2-%): check-$1-$2-%: % $$(IMPL)
	$$(if $$(FORCE_SKIP)$$(SKIP-$$(LIB_STEM)-$$(MCU)-$$<),\
	    $$(warning Target $$@ fails and has been disabled pending investigation),\
	    $$(LUAJIT) $$< "./$$(IMPL)" "$$(LIB)" "$$(MCU)")

vars: mk-gen/vars-$1-$2.mk

mk-gen/vars-$1-$2.mk: lua/suss.lua $$(IMPL) | mk-gen
	$$(LUAJIT) $$< "./$$(IMPL)" "$$(LIB)" "$2" 2> /dev/null | grep = > $$@

$(foreach t,$(TYPES),$(call check_lib_3,$1,$2,$t))
endef

define check_lib_3

check-$1-$2: check-$1-$2-$3-call
check-$1-$2-$3: check-$1-$2-$3-call
check-$1-$2-$3-call: methods.$3.txt

endef

FILES_clean += mk-gen/check-lib-*.mk
ifeq ($(findstring clean,$(MAKECMDGOALS)),)
include $(LIB_LIST:%=mk-gen/check-lib-%.mk)
include $(LIB_LIST:%=mk-gen/mcus-%.mk)
endif

ifneq (4, $(MAKE_VERSION:4.%=4))
    $(error This Makefile uses features that require Make 4.x)
endif

# The use of $(file ...) here requires Make version 4+
$(LIB_LIST:%=mk-gen/check-lib-%.mk): mk-gen/check-lib-%.mk: | mk-gen
	$(file >$@,$(call check_lib_1,$*))

$(foreach t,$(TYPES),$(eval check-%-$t-call: TYPE = $t))
