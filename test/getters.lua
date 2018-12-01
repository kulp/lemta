local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local m2 = core:getModel()

local handle = error
if model._ud == m2._ud then
    handle = print
end

handle("expected = " .. tostring(model._ud) .. ", actual = " .. tostring(m2._ud))

