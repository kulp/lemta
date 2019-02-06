$(MCU_LIST:%=%-cases/break.lua): cases/sum.hex
$(MCU_LIST:%=%-cases/watch.lua): cases/rw.hex

FILES_clean += cases/*.hex
cases/%.hex:
	$(MAKE) -C $(@D) $(@F)
