local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local numregs = core.props[1028].int() -- magic number not yet enumerated in C interface

local write_kinds = {
        -- raw peek/poke interface
        function(c,i,v) c:pokeReg(i,v) end,
        -- simulated-array interface
        function(c,i,v) c.regs[i] = v end,
    }

local read_kinds = {
        -- raw peek/poke interface
        function(c,i)
            local f = ffi.new("unsigned long[1]")
            c:peekReg(i, f)
            return f[0]
        end,
        -- simulated-array interface
        function(c,i) return c.regs[i] end,
    }

for _,put in ipairs(write_kinds) do
    for _,get in ipairs(read_kinds) do
        for i = 0,numregs-1 do
            put(core,i,i * 2)
        end
        for i = 0,numregs-1 do
            local val = get(core,i)
            local handle = error
            if val == i * 2 then
                handle = print
            end

            handle("ok reg " .. i)
        end
    end
end

