$(MCU_LIST:%=%-cases/break.lua): cases/break.hex
$(MCU_LIST:%=%-cases/break.lua): cases/sum.hex
$(MCU_LIST:%=%-cases/watch.lua): cases/rw.hex

FILES_clean += cases/*.hex

# Mark all .hex files as needing to be checked always. Otherwise, because this
# Makefile does not know the dependency graph to build those files, it will
# think they are up-to-date even if their sources change.
.PHONY: FORCE
cases/%.hex: FORCE
	$(MAKE) -C $(@D) $(@F)
