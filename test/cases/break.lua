local Model = require("model/sugared").Model
local Test = require("test")
local Util = require("util")
local ffi = require("ffi")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

--[[
    Test BP_BREAKPOINT
--]]
core:load("sum.hex")

model:reset("RT_TYPE_0") -- otherwise instruction at address 0 is skipped

core:cycles(0) -- reset cycles counter

local add1 = 0x12
local add2 = 0x34

core.regs[16] = add1
core.regs[17] = add2

local nonce = 99
local caught = 0
local kind = "BP_BREAKPOINT"
local id = -1
do
    local bp = core:createBreakpoint({
            addr = 32,
            type = kind,
            segment = "SEG_PROG",
            userdata = ffi.new("char[sizeof(Breakpoint)]", nonce),
        })

    bp.handler = function(_, bp)
        caught = caught + 1
        Test.expect(32, bp.addr)
        Test.expect(1, bp.hitcount)
        Test.expect(32, bp.break_pc)
        Test.expect(nonce, ffi.cast("char*", bp.userdata)[0])
        Test.expect(0, bp.unknown0) -- we do not know what this is for, but let us catch it if it changes
        return "BR_CONTINUE"
    end
    id = core:addBreakpoint(bp)
    Test.expect_not(-1, id)
    Test.expect_not(0, id)
end

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

Test.expect(1, core:removeBreakpoint(id))

--[[
    Test BP_FOUND
--]]
core:load("break.hex")

model:reset("RT_TYPE_0") -- otherwise instruction at address 0 is skipped

core:cycles(0) -- reset cycles counter

core:run(0)
local pc = core.regs["REG_PC"]
Test.expect(34, pc)

-- check that we did not run more cycles than expected
Test.expect(3, core:cycles())

