local Model = require("model/sugared").Model
local ihex = require("ihex")
local Test = require("test")
local Util = require("util")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local prog = core.segments["SEG_PROG"]

function loader(n, addr, kind, data, ck)
    return kind == 0 and prog.write(addr, n, data)
end

ihex.read(io.lines("cases/sum.hex"), loader)

model:reset("RT_TYPE_0") -- otherwise instruction at address 0 is skipped

core:cycles(0) -- reset cycles counter

local add1 = 0x12
local add2 = 0x34

core.regs[16] = add1
core.regs[17] = add2

local caught = 0
local kind = "BP_BREAKPOINT"
local bp = core:createBreakpoint()
bp.addr = 32
bp.type = kind
bp.segment = "SEG_PROG"
bp.handler = function(core, bp)
    caught = caught + 1
    Test.expect(32, bp.addr)
    Test.expect(1, bp.hitcount)
    Test.expect(32, bp.break_pc)
    return 0
end
local id = core:addBreakpoint(bp)
Test.expect(false, id == -1)
Test.expect(false, id == 0)

local list = core:getBreakpoints(kind)
local count = Util.null_terminated_length(list)
Test.expect(1, count)

while core.regs[19] ~= 1 do
    core:step(1)
end

Test.expect(1, caught)

-- check that we did not run more cycles than expected
Test.expect(5, core:cycles())

Test.expect(add1 + add2, core.regs[18])

