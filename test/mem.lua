local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

ffi.cdef[[int memcmp(const void *s1, const void *s2, size_t n);]]

local size = 3
local addr -- set later, per test
local segment = 0
local input = ffi.new("unsigned char[?]", size, { 9, 8, 7 })

function check_read(addr,fashion)
    local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
    fashion(addr, size, output, segment)

    local handle = error
    if ffi.C.memcmp(input,output,size) == 0 then
        handle = print
    end

    handle("input = " .. tostring(input) .. ", output = " .. tostring(output))
end

-- write method 1 : raw call to writeMemory
addr = 0x102
core:writeMemory(addr, size, input, segment)

check_read(addr,function(a,s,o,p) core:readMemory(a,s,o,p) end)
check_read(addr,function(a,s,o,p) core.segments[p].read(a,s,o) end)
check_read(addr,function(a,s,o,p) for i = 0,s-1 do o[i] = core.segments[p].mem[a + i] end end)

-- write method 2 : segment façade with .write call
addr = addr - 10
core.segments[segment].write(addr, size, input)

check_read(addr,function(a,s,o,p) core:readMemory(a,s,o,p) end)
check_read(addr,function(a,s,o,p) core.segments[p].read(a,s,o) end)
check_read(addr,function(a,s,o,p) for i = 0,s-1 do o[i] = core.segments[p].mem[a + i] end end)

-- write method 3 : segment façade with mem façade
addr = addr - 20
for i = 0,size-1 do
    core.segments[segment].mem[addr + i] = input[i]
end

check_read(addr,function(a,s,o,p) core:readMemory(a,s,o,p) end)
check_read(addr,function(a,s,o,p) core.segments[p].read(a,s,o) end)
check_read(addr,function(a,s,o,p) for i = 0,s-1 do o[i] = core.segments[p].mem[a + i] end end)

