AS = avr-gcc
CC = avr-gcc

MCUFLAGS = $(if $(MCU),-mmcu=$(MCU))
CFLAGS  += $(MCUFLAGS)
ASFLAGS += $(MCUFLAGS)
LDFLAGS += $(MCUFLAGS)

VPATH += src

rw.%: MCU = attiny84a
sum.%: MCU =# none

%.elf: %.o
	$(LINK.o) -o $@ $^

FLASH_SECTIONS = text data vectors
%.hex: %.elf
	avr-objcopy $(FLASH_SECTIONS:%=-j .%) -O ihex $< $@
