local Model = require("model")
local Test = require("test")
local Util = require("util")
local ffi = require("ffi")

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
    local function make_addr(i) return 100 * kind + i end -- arbitrary
    local ids = {}
    for i = 0, max - 1 do
        local bp = core:createBreakpoint()
        bp.addr = make_addr(i)
        bp.size = 1 -- must be nonzero for BP_TRACEPOINT
        bp.type = kind
        bp.segment = segs[kind]
        bp.debugName = "" -- non-zero-length strings crash inside dbgVarFind due to null VerilatedScope
        bp.userdata = ffi.cast("void *", bp.addr * 10)
        local id = core:addBreakpoint(bp)
        ids[#ids + 1] = id
        Test.expect(false, id == -1)
        Test.expect(false, id == 0)

        -- try to add the same breakpoint a second time
        id = core:addBreakpoint(bp)
        Test.expect(-1, id)
    end

    local list = core:getBreakpoints(kind)
    local count = Util.null_terminated_length(list)
    Test.expect(max, count)

    for i = 0, count - 1 do
        Test.expect(make_addr(i), list[i].addr)
        Test.expect(kind, list[i].type)
        Test.expect(ffi.cast("void *", make_addr(i) * 10), list[i].userdata)
    end

    Test.expect(1, core:removeBreakpoint(ids[max - 2])) -- success first time

    Test.expect(max - 1, Util.null_terminated_length(core:getBreakpoints(kind)))
    Test.expect(-1, core:removeBreakpoint(ids[max - 2])) -- failure second time
end

