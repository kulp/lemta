local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))

local cycles = 0
local count = 10

local function cycle_cb(model)
    cycles = cycles + 1
end

model:addCycleCallback(cycle_cb)
model:cycle(count)

local handle = error
if cycles == count then
    handle = print
end

handle("expected = " .. count .. ", actual = " .. cycles)

