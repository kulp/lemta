local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local segment = core.segments["SEG_REGS"]

for i = 0,31 do
    local input = 0xff - i
    segment.mem[i] = input
    local output = core.regs[i]

    local handle = error
    if input == output then
        handle = print
    end

    handle("input = " .. input .. ", output = " .. tostring(output))
end

