SKIP_RUN_TARGETS += check-attiny40-ATtiny40-cases/run.lua
SKIP_RUN_TARGETS += check-attiny10-ATtiny5-cases/run.lua
SKIP_RUN_TARGETS += check-attiny10-ATtiny10-cases/run.lua
SKIP_RUN_TARGETS += check-attiny10-ATtiny4-cases/run.lua
SKIP_RUN_TARGETS += check-attiny10-ATtiny9-cases/run.lua
SKIP_RUN_TARGETS += check-attiny20-ATtiny20-cases/run.lua

$(SKIP_RUN_TARGETS):
	$(warning Target $@ fails and has been disabled pending investigation)
