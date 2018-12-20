local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local m2 = core:getModel()

local handle = error
if model == m2 then
    handle = function() end
end

handle("expected = " .. tostring(model) .. ", actual = " .. tostring(m2))

