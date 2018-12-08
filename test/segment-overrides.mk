SEGMENT_UPPER_BOUND_TARGETS += check-attinyxb-ATtiny202-cases/eeprom.lua
SEGMENT_UPPER_BOUND_TARGETS += check-attinyxb-ATtiny204-cases/eeprom.lua
SEGMENT_UPPER_BOUND_TARGETS += check-attinyxb-ATtiny214-cases/eeprom.lua
SEGMENT_UPPER_BOUND_TARGETS += check-attinyxb-ATtiny212-cases/eeprom.lua
SEGMENT_UPPER_BOUND_TARGETS += check-attinyxg-ATmega808-cases/eeprom.lua
SEGMENT_UPPER_BOUND_TARGETS += check-attinyxg-ATmega809-cases/eeprom.lua
SEGMENT_UPPER_BOUND_TARGETS += check-atmegaapb-ATMEGA48PB-cases/eeprom.lua
SEGMENT_UPPER_BOUND_TARGETS += check-attinyxf-ATtiny807-cases/eeprom.lua
SEGMENT_UPPER_BOUND_TARGETS += check-attinyxf-ATtiny806-cases/eeprom.lua
SEGMENT_UPPER_BOUND_TARGETS += check-attinyxf-ATtiny804-cases/eeprom.lua

$(SEGMENT_UPPER_BOUND_TARGETS): export SKIP_UPPER_BOUND_CHECK_SEGMENT_SEG_EEPROM = 1

SEGMENT_LOWER_BOUND_TARGETS += check-attinyxe-ATmega3208-cases/eeprom.lua
SEGMENT_LOWER_BOUND_TARGETS += check-attinyxe-ATmega3209-cases/eeprom.lua
SEGMENT_LOWER_BOUND_TARGETS += check-attinyxe-ATmega4809-cases/eeprom.lua
SEGMENT_LOWER_BOUND_TARGETS += check-attinyxe-ATmega4808-cases/eeprom.lua

$(SEGMENT_LOWER_BOUND_TARGETS): export SKIP_LOWER_BOUND_CHECK_SEGMENT_SEG_EEPROM = 1
