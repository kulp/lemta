local ffi = require("ffi")
local Model = require("model")
local ihex = require("lua/ihex")

local mcu = arg[1]
local base = "cases/sum"
local hex = base .. ".hex"

local model = Model:create(unpack(arg))
local core = model:getCore(0)

local function make_loader(segment)
    return function(n,addr,kind,data,ck)
        if kind == 0 then
            segment.write(addr,n,data)
        end
    end
end

ihex.read(io.lines(hex),make_loader(core.segments["SEG_PROG"]))

local add1 = 0x12
local add2 = 0x34

core.regs[16] = add1
core.regs[17] = add2

while core.regs[19] ~= 1 do
    core:step(1)
end

local handle = (add1 + add2 == core.regs[18] and print or error)
handle(string.format("expected 0x%02x, got 0x%02x", add1 + add2, core.regs[18]))

