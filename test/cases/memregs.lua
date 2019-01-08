local Model = require("model/sugared").Model
local Test = require("test")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local segment = core.segments["SEG_REGS"]
local numregs = core.regs.len()

for j = 0, 31, numregs do -- 16-register cores have their registers mapped twice
    for i = 0, numregs-1 do
        local r = i + j
        local input = 0xff - r
        segment.mem[r] = input
        local output = core.regs[r]

        Test.expect(input, output)
    end
end

