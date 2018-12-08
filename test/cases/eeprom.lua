local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

ffi.cdef[[int memcmp(const void *s1, const void *s2, size_t n);]]

local temp = ffi.new("unsigned long[1]", 0)
-- integer property index 9 appears to be the size of the EEPROM in segment 2
local width = model:getIntProperty(9, temp, nil)
local size = tonumber(temp[0])
local segment = "SEG_EEPROM"
local input = ffi.new("unsigned char[?]", size * 2)

temp[0] = 0
-- integer property index 10 appears to be the base address of the EEPROM in segment 2
model:getIntProperty(10, temp, nil)
local addr = tonumber(temp[0])

for i = 0,size*2-1 do
    input[i] = 0xff - i
end

function check_read(addr, size, input, fashion, invert)
    local output = ffi.new("unsigned char[?]", size)
    for i = 0,size-1 do
        output[i] = 0xa5
    end

    fashion(addr, size, output, segment)

    local handle = error
    local equal = ffi.C.memcmp(input, output, size) == 0
    local normal  = not invert and     equal
    local flipped =     invert and not equal
    if normal or flipped then
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

do
    local env = os.getenv("SKIP_UPPER_BOUND_CHECK_SEGMENT_" .. segment)
    if tonumber(env) ~= 1 then
        -- ensure segment is not larger than advertised
        for i,put in ipairs(write_kinds) do
            put(addr, size + 1, input, segment)
            for _,get in ipairs(read_kinds) do
                check_read(addr, size + 1, input, get, true)
            end
        end
    end
end

do
    local env = os.getenv("SKIP_LOWER_BOUND_CHECK_SEGMENT_" .. segment)
    if tonumber(env) ~= 1 then
        -- ensure segment is at least as large as advertised
        for i,put in ipairs(write_kinds) do
            put(addr, size, input, segment)
            for _,get in ipairs(read_kinds) do
                check_read(addr, size, input, get)
            end
        end
    end
end

