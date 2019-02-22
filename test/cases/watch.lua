local Model = require("model/sugared").Model
local Test = require("test")
local Util = require("util")
local ffi = require("ffi")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

core:load("rw.hex")

model:reset("RT_TYPE_0") -- otherwise instruction at address 0 is skipped

core:cycles(0) -- reset cycles counter

local addr_a = 0x60
local addr_main = 0x44

core:run(addr_main)

local nonce = 999
local caught = 0
local kind = "BP_WATCH_WRITE"
local bp = core:createBreakpoint()
bp.addr = addr_a
bp.addr2 = addr_a
bp.size = 1
bp.type = kind
bp.segment = "SEG_DATA"
bp.userdata = ffi.new("int[1]", nonce)
bp.handler = function(core, bp)
    caught = caught + 1
    Test.expect(addr_a, bp.addr)
    Test.expect(1, bp.hitcount)
    Test.expect(nonce, ffi.cast("int*", bp.userdata)[0])
    -- break_pc apparently does not get set for a watch
    Test.expect(0, bp.unknown0) -- we do not know what this is for, but let us catch it if it changes
    return "BR_CONTINUE"
end
local id = core:addBreakpoint(bp)
Test.expect_not(-1, id)
Test.expect_not(0, id)

local list = core:getBreakpoints(kind)
local count = Util.null_terminated_length(list)
Test.expect(1, count)

local function get_pc() return core.regs["REG_PC"] end
local last, pc = get_pc()
repeat
    last = pc
    core:step(1)
    pc = get_pc()
until pc == 0 or pc == last

Test.expect(1, caught)

