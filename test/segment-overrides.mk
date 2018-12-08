EEPROM_UPPER_BOUND_TARGETS += check-attinyxb-ATtiny202-cases/segments.lua
EEPROM_UPPER_BOUND_TARGETS += check-attinyxb-ATtiny204-cases/segments.lua
EEPROM_UPPER_BOUND_TARGETS += check-attinyxb-ATtiny214-cases/segments.lua
EEPROM_UPPER_BOUND_TARGETS += check-attinyxb-ATtiny212-cases/segments.lua
EEPROM_UPPER_BOUND_TARGETS += check-attinyxg-ATmega808-cases/segments.lua
EEPROM_UPPER_BOUND_TARGETS += check-attinyxg-ATmega809-cases/segments.lua
EEPROM_UPPER_BOUND_TARGETS += check-atmegaapb-ATMEGA48PB-cases/segments.lua
EEPROM_UPPER_BOUND_TARGETS += check-attinyxf-ATtiny807-cases/segments.lua
EEPROM_UPPER_BOUND_TARGETS += check-attinyxf-ATtiny806-cases/segments.lua
EEPROM_UPPER_BOUND_TARGETS += check-attinyxf-ATtiny804-cases/segments.lua

$(EEPROM_UPPER_BOUND_TARGETS): export SKIP_UPPER_BOUND_CHECK_SEGMENT_SEG_EEPROM = 1

EEPROM_LOWER_BOUND_TARGETS += check-attinyxe-ATmega3208-cases/segments.lua
EEPROM_LOWER_BOUND_TARGETS += check-attinyxe-ATmega3209-cases/segments.lua
EEPROM_LOWER_BOUND_TARGETS += check-attinyxe-ATmega4809-cases/segments.lua
EEPROM_LOWER_BOUND_TARGETS += check-attinyxe-ATmega4808-cases/segments.lua

$(EEPROM_LOWER_BOUND_TARGETS): export SKIP_LOWER_BOUND_CHECK_SEGMENT_SEG_EEPROM = 1

DATA_TARGETS += check-attinyxe-ATmega3208-cases/segments.lua
DATA_TARGETS += check-attinyxe-ATmega3209-cases/segments.lua
DATA_TARGETS += check-attinyxe-ATmega4809-cases/segments.lua
DATA_TARGETS += check-attinyxe-ATmega4808-cases/segments.lua
DATA_TARGETS += check-atmegaapb-ATMEGA48PB-cases/segments.lua

$(DATA_TARGETS): export SKIP_UPPER_BOUND_CHECK_SEGMENT_SEG_DATA = 1
