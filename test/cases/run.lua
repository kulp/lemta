local ffi = require("ffi")
local Model = require("model")
local ihex = require("lua/ihex")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local prog = core.segments["SEG_PROG"]

function loader(n, addr, kind, data, ck)
    return kind == 0 and prog.write(addr, n, data)
end

ihex.read(io.lines("cases/sum.hex"), loader)

local add1 = 0x12
local add2 = 0x34

core.regs[16] = add1
core.regs[17] = add2

while core.regs[19] ~= 1 do
    core:step(1)
end

local handle = (add1 + add2 == core.regs[18] and function() end or error)
handle(string.format("expected 0x%02x, got 0x%02x", add1 + add2, core.regs[18]))

