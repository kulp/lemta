local Test = require("test")

local expected = 10

-- cannot require them both up front because sugared actually modifies the
-- table imported from base
local model_paths = { "model/base", "model/sugared" }

local transformations = {
    {
        adder   = "addCycleCallback",
        finder  = function (model) return model end,
        invoker = function (obj, ...) obj:cycle(...) end,
    },
    {
        adder   = "addStepCallback",
        finder  = function (model) return model:getCore(0) end,
        invoker = function (obj, ...) obj:step(...) end,
    },
}

local arities = {
    function (obj, meth, cb) obj[meth](obj, cb) end,
    function (obj, meth, cb) obj[meth](obj, cb, nil) end,
}

local results = {
    { false, true },
    { true, true },
}

function run_test(model, transform, add_cb)
    model:reset("RT_TYPE_0")

    local achieved = 0

    local function achiever(_,_)
        achieved = achieved + 1
    end

    local obj = transform.finder(model)
    add_cb(obj, transform.adder, achiever)
    transform.invoker(obj, expected)

    Test.expect(expected, achieved)
end

for i, path in ipairs(model_paths) do
    -- do the "require" as late as possible, because later requires overwrite
    -- state that earlier requires provide
    local model = require(path).Model:create(unpack(arg))
    for _, tf in ipairs(transformations) do
        for j, cb in ipairs(arities) do
            local result = pcall(function () run_test(model, tf, cb) end)
            Test.expect(results[i][j], result)
        end
    end
end

