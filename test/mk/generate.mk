define check_lib_1

check: check-$1
check-$1 check-$1-%: LIB_STEM = $1

$(foreach m,$(call MCU_NAMES,$1),$(call check_lib_2,$1,$m))
endef

define check_lib_2

check-$1: check-$1-$2
check-$1-$2 check-$1-$2-%: MCU = $2

MCU_LIST += check-$1-$2
check-$1-$2: $(LUA_TESTS:%=check-$1-$2-%)
$(LUA_TESTS:%=check-$1-$2-%): check-$1-$2-%: % | $$(IMPL)
	$$(if $$(SKIP-$$(LIB_STEM)-$$(MCU)-$$<),\
	    $$(warning Target $$@ fails and has been disabled pending investigation),\
	    $$(LUAJIT) $$< ./$$| $$(LIB) $$(MCU))

vars: mk-gen/vars-$1-$2.mk

mk-gen/vars-$1-$2.mk: lua/suss.lua | $$(IMPL) mk-gen
	$$(LUAJIT) $$< ./$$(firstword $$|) lib$1.so $2 2> /dev/null | grep = > $$@

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

# The use of $(file ...) here requires Make version 4+
$(LIB_LIST:%=mk-gen/check-lib-%.mk): mk-gen/check-lib-%.mk: | mk-gen
	$(file >$@,$(call check_lib_1,$*))

