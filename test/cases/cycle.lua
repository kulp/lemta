local Test = require("test")

-- cannot require them both up front because sugared actually modifies the
-- table imported from base
local model_paths = { "model/base", "model/sugared" }

local results = {
    { false, true },
    { true, true },
}

function add_cb_1(model, cb) model:addCycleCallback(cb) end
function add_cb_2(model, cb) model:addCycleCallback(cb, nil) end

function run_test(Model, add_cb)
    local model = Model:create(unpack(arg))

    local cycles = 0
    local count = 10

    local function cycle_cb(model)
        cycles = cycles + 1
    end

    add_cb(model, cycle_cb)
    model:cycle(count)

    Test.expect(count, cycles)
end

for i, path in ipairs(model_paths) do
    local Model = require(path).Model
    for j, cb in ipairs({ add_cb_1, add_cb_2 }) do
        local result = pcall(function () run_test(Model, cb) end)
        Test.expect(results[i][j], result)
    end
end

