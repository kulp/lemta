local Model = require("model/sugared").Model
local Test = require("test")
local Util = require("util")
local ffi = require("ffi")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

local max = 5

local segs = {
    BP_BREAKPOINT  = "SEG_PROG",
    BP_WATCH_WRITE = "SEG_DATA",
    BP_WATCH_READ  = "SEG_DATA",
    BP_TRACEPOINT  = "SEG_PROG",
    BP_FOUND       = "SEG_PROG",
}

local bptypes = {
    "BP_BREAKPOINT",
    "BP_WATCH_WRITE",
    "BP_WATCH_READ",
    --"BP_TRACEPOINT", -- TODO make BP_TRACEPOINT work (addBreakpoint fails in some cases for unknown reasons)
    --"BP_FOUND", -- BP_FOUND is not appropriate for looping over
}

for _, kind in ipairs(bptypes) do
    local function make_addr(i) return 100 * tonumber(ffi.cast("enum BPtype", kind)) + i end -- arbitrary
    local ids = {}
    for i = 0, max - 1 do
        local bp = core:createBreakpoint()
        bp.addr = make_addr(i)
        bp.size = 1 -- must be nonzero for BP_TRACEPOINT
        bp.type = kind
        bp.segment = segs[kind]
        bp.debug_name = "" -- non-zero-length strings crash inside dbgVarFind due to null VerilatedScope
        bp.userdata = ffi.cast("void *", bp.addr * 10)
        local id = core:addBreakpoint(bp)
        ids[#ids + 1] = id
        Test.expect_not(-1, id)
        Test.expect_not(0, id)

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

