local Model = require("model/sugared").Model
local Test = require("test")
local Util = require("util")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

core:load("io.hex")

model:reset("RT_TYPE_0") -- otherwise instruction at address 0 is skipped

local caught = 0

function make_watchpoint(kind, io_addr)
    local bp = core:createBreakpoint({
            addr = io_addr,
            addr2 = io_addr,
            size = 1,
            segment = "SEG_IO",
            type = kind,
        })

    bp.handler = function(_, bp)
        caught = caught + 1
        Test.expect(io_addr, bp.addr2)
        Test.expect(1, bp.hitcount)
        Test.expect(32, bp.break_pc)
        return "BR_TYPE_1"
    end
    local id = core:addBreakpoint(bp)
    Test.expect_not(-1, id)
    Test.expect_not(0, id)

    local list = core:getBreakpoints(kind)
    local count = Util.null_terminated_length(list)
    Test.expect(1, count)
end

make_watchpoint("BP_WATCH_READ", 12) -- arbitrary I/O addr is determined by io.S source code

core:run(0)

Test.expect(1, caught)

