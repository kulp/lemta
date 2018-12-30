local ffi = require("ffi")
local Model = require("model")
local Test = require("test")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

local max = 5

local segs = {
    [1] = "SEG_PROG",
    [2] = "SEG_DATA",
    [4] = "SEG_DATA",
    [8] = "SEG_PROG",
}

-- TODO make BPType 8 work (addBreakpoint fails in some cases for unknown reasons)
for _, kind in ipairs({ 1, 2, 4 }) do
    local ids = {}
    for i = 1, max do
        local bp = ffi.new("Breakpoint")
        bp.addr = 100 * kind + i
        bp.size = 1 -- must be nonzero for BP_TRACEPOINT
        bp.type = kind
        bp.segment = segs[kind]
        local id = core:addBreakpoint(bp)
        ids[#ids + 1] = id
        Test.expect(false, id == -1)
        Test.expect(false, id == 0)
    end

    local list = core:getBreakpoints(kind)

    local function null_terminated(list)
        local i = 0
        return function()
            local result = list[i]
            if result ~= nil then
                i = i + 1
                return result
            end
        end
    end

    local count = 0
    for v in null_terminated(list) do
        count = count + 1
    end

    Test.expect(max, count)
    Test.expect( 1, core:removeBreakpoint(ids[max - 2])) -- success first time
    Test.expect(-1, core:removeBreakpoint(ids[max - 2])) -- failure second time
end

