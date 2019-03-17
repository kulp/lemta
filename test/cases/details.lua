local Model = require("model/base").Model
local Test = require("test")

local model = Model:create(unpack(arg))
local actual = model._ver
local expected = 0x70000

Test.expect(expected, actual)
Test.expect_not(nil, model._lib)
Test.expect_not(nil, model._impl)
