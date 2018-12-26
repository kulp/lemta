local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local numregs = core.regs.len()

local write_kinds = {
        -- raw peek/poke interface
        function(c, i, v) c:pokeReg(i, v) end,
        -- simulated-array interface
        function(c, i, v) c.regs[i] = v end,
    }

local read_kinds = {
        -- raw peek/poke interface
        function(c, i)
            local f = ffi.new("unsigned long[1]")
            c:peekReg(i, f)
            return f[0]
        end,
        -- simulated-array interface
        function(c, i) return c.regs[i] end,
    }

for _, put in ipairs(write_kinds) do
    for _, get in ipairs(read_kinds) do
        for j = 0, 31, numregs do -- 16-register cores have their registers mapped twice
            for i = 0, numregs-1 do
                local r = i + j
                put(core, r, r * 2)
            end
            for i = 0, numregs-1 do
                local r = i + j
                local val = get(core, r)
                local handle = error
                if val == r * 2 then
                    handle = function() end
                end

                handle("ok reg " .. r)
            end
        end
    end
end

