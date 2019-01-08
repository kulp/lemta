local Model = require("model/sugared").Model
local Test = require("test")

local model = Model:create(unpack(arg))
local actual = model._ver
local expected = 0x70000

Test.expect(expected, actual)
