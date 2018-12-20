local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local actual = model._ver
local expected = 0x70000

local handle = error
if expected == actual then
    handle = function() end
end

handle(string.format("expected = 0x%06x, actual = 0x%06x", expected, actual))

