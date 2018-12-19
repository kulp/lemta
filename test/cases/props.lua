local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

for _,g in next,{ core, model } do
    -- simulated-array interface
    local output = g.props[1028] -- number of registers

    local handle = error
    local skip = g == model and os.getenv("SKIP_MODEL_PROP_CHECK") == "1"
    if skip or output.int() == 16 or output.int() == 32 then
        handle = print
    end

    handle("output = '" .. tostring(output) .. "' = " .. output.int())
    print("prop = " .. tostring(g.props[0]))

    -- Currently we do not have a way to test setting properties, except by
    -- ensuring that doing so does not crash us
    g.props[1028] = 31
    g.props[1028] = "32"
end

