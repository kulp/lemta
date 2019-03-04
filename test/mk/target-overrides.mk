$(MCU_LIST:%=check-%-cases/break.lua): break.hex
$(MCU_LIST:%=check-%-cases/break.lua): sum.hex
$(MCU_LIST:%=check-%-cases/watch.lua): rw.hex
$(MCU_LIST:%=check-%-cases/io.lua): io.hex
$(MCU_LIST:%=check-%-cases/io.lua): FORCE_SKIP = 1

# Only a few combinations currently work with io.S
check-attiny10-ATtiny10-cases/io.lua: FORCE_SKIP =#off
check-attiny10-ATtiny4-cases/io.lua: FORCE_SKIP =#off
check-attiny10-ATtiny5-cases/io.lua: FORCE_SKIP =#off
check-attiny10-ATtiny9-cases/io.lua: FORCE_SKIP =#off
check-attiny20-ATtiny20-cases/io.lua: FORCE_SKIP =#off
check-attiny40-ATtiny40-cases/io.lua: FORCE_SKIP =#off
check-atxmega128a1u-ATxmega64A1u-cases/io.lua: FORCE_SKIP =#off
check-atxmega128a4-ATxmega64A4U-cases/io.lua: FORCE_SKIP =#off
check-atxmega128a4-ATxmega64D4-cases/io.lua: FORCE_SKIP =#off
check-atxmega128b1-ATxmega64B3-cases/io.lua: FORCE_SKIP =#off
check-atxmega256a3u-ATxmega64A3-cases/io.lua: FORCE_SKIP =#off
check-atxmega256a3u-ATxmega64A3u-cases/io.lua: FORCE_SKIP =#off
check-atxmega256a3u-ATxmega64D3-cases/io.lua: FORCE_SKIP =#off
check-atxmega32a4u-ATxmega16A4-cases/io.lua: FORCE_SKIP =#off
check-atxmega32a4u-ATxmega16A4U-cases/io.lua: FORCE_SKIP =#off
check-atxmega32a4u-ATxmega16D4-cases/io.lua: FORCE_SKIP =#off
check-atxmega32a4u-ATxmega32A4-cases/io.lua: FORCE_SKIP =#off
check-atxmega32a4u-ATxmega32A4U-cases/io.lua: FORCE_SKIP =#off
check-atxmega32a4u-ATxmega32D4-cases/io.lua: FORCE_SKIP =#off
check-atxmega32c4-ATxmega16C4-cases/io.lua: FORCE_SKIP =#off
check-atxmega32c4-ATxmega32C4-cases/io.lua: FORCE_SKIP =#off
check-atxmega64c3-ATxmega32C3-cases/io.lua: FORCE_SKIP =#off
check-atxmega64c3-ATxmega32D3-cases/io.lua: FORCE_SKIP =#off
check-atxmega64c3-ATxmega64C3-cases/io.lua: FORCE_SKIP =#off
check-atxmegae5-ATxmega16E5-cases/io.lua: FORCE_SKIP =#off
check-atxmegae5-ATxmega32E5-cases/io.lua: FORCE_SKIP =#off
check-atxmegae5-ATxmega8E5-cases/io.lua: FORCE_SKIP =#off

