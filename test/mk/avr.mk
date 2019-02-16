%.elf: AS = avr-gcc
%.elf: CC = avr-gcc
# use the C linker for AVR targets
%.elf: LINK.o = $(LINK.c)

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
	$(LINK.o) -o $@ $^ $(LDLIBS)

FILES_clean += *.hex
FLASH_SECTIONS = text data vectors
%.hex: %.elf
	avr-objcopy $(FLASH_SECTIONS:%=-j .%) -O ihex $< $@

