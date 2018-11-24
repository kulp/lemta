local ffi = require("ffi")
local Model = require("model")

local model = Model:create("attinyxc", "ATtiny1617")
local core = model:getCore(0)

-- raw peek/poke interface
local input = 0x12
core:pokeReg(3, input)
local output = ffi.new("unsigned long[1]")
local result = core:peekReg(3, output)

local handle = error
if result == 1 and input == output[0] then
    handle = print
end

handle("result = " .. result .. ", input = " .. input .. ", output = " .. tostring(output[0]))

-- simulated-array interface
local input = 0x15
core.regs[9] = input
local output = core.regs[9]

local handle = error
if input == output then
    handle = print
end

handle("result = " .. result .. ", input = " .. input .. ", output = " .. tostring(output))

