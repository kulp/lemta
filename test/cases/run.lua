local Model = require("model")
local ihex = require("ihex")
local Test = require("test")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local prog = core.segments["SEG_PROG"]

function loader(n, addr, kind, data, ck)
    return kind == 0 and prog.write(addr, n, data)
end

ihex.read(io.lines("cases/sum.hex"), loader)

local before = core.regs[0x104]

model:reset(0) -- otherwise instruction at address 0 is skipped

local add1 = 0x12
local add2 = 0x34

core.regs[16] = add1
core.regs[17] = add2

while core.regs[19] ~= 1 do
    core:step(1)
end

local after = core.regs[0x104]

-- check that we did not run more cycles than expected
Test.expect(5, after - before)

Test.expect(add1 + add2, core.regs[18])

