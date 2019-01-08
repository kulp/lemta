local Model = require("model/sugared").Model
local Test = require("test")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

local steps = 0
local count = 10

local function step_cb(core)
    steps = steps + 1
end

core:addStepCallback(step_cb)
core:step(count)

Test.expect(count, steps)
