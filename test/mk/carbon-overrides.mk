NEED_CARBON += atmega1280
NEED_CARBON += atmega16a
NEED_CARBON += atmega256
NEED_CARBON += atmega32a
NEED_CARBON += atmega32hvb
NEED_CARBON += atmega64a
NEED_CARBON += atmega8a
NEED_CARBON += attiny10
NEED_CARBON += attiny1661
NEED_CARBON += attiny20
NEED_CARBON += attiny3234
NEED_CARBON += attiny40
NEED_CARBON += attiny4313
NEED_CARBON += attiny828q
NEED_CARBON += attiny84a
NEED_CARBON += atxmega128a1u
NEED_CARBON += atxmega128b1
NEED_CARBON += atxmega256a3u
NEED_CARBON += atxmega32c4
NEED_CARBON += atxmega384c3
NEED_CARBON += atxmega64c3

$(NEED_CARBON:%=check-%-%): export LD_LIBRARY_PATH = $(UNPACKED)/$(LIB_DIR)

