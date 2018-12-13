SKIP_REGS_TARGETS += check-atmegaapb-ATMEGA168PB-cases/memregs.lua
SKIP_REGS_TARGETS += check-atmegaapb-ATMEGA48PB-cases/memregs.lua
SKIP_REGS_TARGETS += check-atmegaapb-ATMEGA88PB-cases/memregs.lua
SKIP_REGS_TARGETS += check-atmegaipb-ATMEGA324PB-cases/memregs.lua
SKIP_REGS_TARGETS += check-atmegakpb-ATMEGA328PB-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyp-ATTINY102-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyp-ATTINY104-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxa-ATtiny417-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxa-ATtiny814-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxa-ATtiny816-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxa-ATtiny817-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxb-ATtiny202-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxb-ATtiny204-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxb-ATtiny212-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxb-ATtiny214-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxb-ATtiny402-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxb-ATtiny404-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxb-ATtiny406-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxb-ATtiny412-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxb-ATtiny414-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxb-ATtiny416-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxc-ATtiny1614-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxc-ATtiny1616-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxc-ATtiny1617-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxd-ATtiny3214-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxd-ATtiny3216-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxd-ATtiny3217-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxe-ATmega3208-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxe-ATmega3209-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxe-ATmega4808-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxe-ATmega4809-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxf-ATtiny1604-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxf-ATtiny1606-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxf-ATtiny1607-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxf-ATtiny804-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxf-ATtiny806-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxf-ATtiny807-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxg-ATmega1608-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxg-ATmega1609-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxg-ATmega808-cases/memregs.lua
SKIP_REGS_TARGETS += check-attinyxg-ATmega809-cases/memregs.lua

$(SKIP_REGS_TARGETS):
	$(warning Target $@ fails and has been disabled pending investigation)

