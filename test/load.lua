local Model = require("model")

local model = Model:create("attinyxc")
local core = model:getCore(0)
local m = core:getModel()

core:stop()
model:stop()
m:stop()

