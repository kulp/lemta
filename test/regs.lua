local ffi = require("ffi")
local Model = require("model")

local model = Model:create("attinyxc")
local core = model:getCore(0)

local input = 0x12
core:pokeReg(3, input)
local output = ffi.new("unsigned long[1]", 0)
local result = core:peekReg(3, output)

local handle = error
if result == 1 and input == output[0] then
    handle = print
end

handle("result = " .. result .. ", input = " .. input .. ", output = " .. tostring(output[0]))

