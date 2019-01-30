local Models = {
    require("model/base").Model,
    require("model/sugared").Model,
}

for _, Model in ipairs(Models) do
    local Test = require("test")

    local model = Model:create(unpack(arg))

    local cycles = 0
    local count = 10

    local function cycle_cb(model)
        cycles = cycles + 1
    end

    model:addCycleCallback(cycle_cb, nil)
    model:cycle(count)

    Test.expect(count, cycles)
end
