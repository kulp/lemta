local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

-- simulated-array interface
local output = core.props[1028] -- number of registers

local handle = error
if output.int() == 16 or output.int() == 32 then
    handle = print
end

handle("output = '" .. tostring(output) .. "' = " .. output.int())
print("core = " .. tostring(core.props[0]))

