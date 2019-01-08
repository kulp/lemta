local Model = require("model/sugared").Model
local Test = require("test")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local m2 = core:getModel()

Test.expect(model, m2)
