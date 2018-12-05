local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

ffi.cdef[[int memcmp(const void *s1, const void *s2, size_t n);]]

local size = 3
local segment = 0
local input = ffi.new("unsigned char[?]", size, { 9, 8, 7 })

function check_read(addr, fashion)
    local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
    fashion(addr, size, output, segment)

    local handle = error
    if ffi.C.memcmp(input, output, size) == 0 then
        handle = print
    end

    handle("input = " .. tostring(input) .. ", output = " .. tostring(output))
end

local write_kinds = {
        function(a,s,m,p) core:writeMemory(a,s,m,p) end,
        function(a,s,m,p) core.segments[p].write(a,s,m) end,
        function(a,s,m,p) for i = 0,s-1 do core.segments[p].mem[a + i] = m[i] end end,
    }

local read_kinds = {
        function(a,s,m,p) core:readMemory(a,s,m,p) end,
        function(a,s,m,p) core.segments[p].read(a,s,m) end,
        function(a,s,m,p) for i = 0,s-1 do m[i] = core.segments[p].mem[a + i] end end,
    }

for i,put in ipairs(write_kinds) do
    local addr = 0x10 + i * size
    put(addr, size, input, segment)
    for _,get in ipairs(read_kinds) do
        check_read(addr, get)
    end
end

