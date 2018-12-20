local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local segment = core.segments["SEG_REGS"]
local numregs = core.props[1028].int() -- magic number not yet enumerated in C interface

for j = 0,31,numregs do -- 16-register cores have their registers mapped twice
    for i = 0,numregs-1 do
        local r = i + j
        local input = 0xff - r
        segment.mem[r] = input
        local output = core.regs[r]

        local handle = error
        if input == output then
            handle = function() end
        end

        handle("input = " .. input .. ", output = " .. tostring(output))
    end
end

