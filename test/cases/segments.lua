local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local segment_names = { "SEG_EEPROM", "SEG_DATA" }

ffi.cdef[[int memcmp(const void *s1, const void *s2, size_t n);]]

function check_read(core, segment, addr, size, input, fashion, invert)
    local output = ffi.new("unsigned char[?]", size)
    for i = 0,size-1 do
        output[i] = 0xa5
    end

    fashion(core, addr, size, output, segment)

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
        function(c,a,s,m,p) c:writeMemory(a,s,m,p) end,
        function(c,a,s,m,p) c.segments[p].write(a,s,m) end,
        function(c,a,s,m,p) for i = 0,s-1 do c.segments[p].mem[a + i] = m[i] end end,
    }

local read_kinds = {
        function(c,a,s,m,p) c:readMemory(a,s,m,p) end,
        function(c,a,s,m,p) c.segments[p].read(a,s,m) end,
        function(c,a,s,m,p) for i = 0,s-1 do m[i] = c.segments[p].mem[a + i] end end,
    }

for _,segment_name in ipairs(segment_names) do
    local segment = ffi.cast("Segment", segment_name)
    local temp = ffi.new("unsigned long[1]", 0)
    -- integer property index N*2+5 appears to be the size of segment N
    local width = model:getIntProperty(segment * 2 + 5, temp, nil)
    local size = tonumber(temp[0])
    if size > 0 then
        local input = ffi.new("unsigned char[?]", size * 2)

        temp[0] = 0
        -- integer property index N*2+6 appears to be the base address of segment N
        model:getIntProperty(segment * 2 + 6, temp, nil)
        local addr = tonumber(temp[0])

        for i = 0,size*2-1 do
            input[i] = 0xff - i
        end

        do
            local env = os.getenv("SKIP_UPPER_BOUND_CHECK_SEGMENT_" .. segment_name)
            if tonumber(env) ~= 1 then
                -- ensure segment is not larger than advertised
                for i,put in ipairs(write_kinds) do
                    put(core, addr, size + 1, input, segment)
                    for _,get in ipairs(read_kinds) do
                        check_read(core, segment, addr, size + 1, input, get, true)
                    end
                end
            end
        end

        do
            local env = os.getenv("SKIP_LOWER_BOUND_CHECK_SEGMENT_" .. segment_name)
            if tonumber(env) ~= 1 then
                -- ensure segment is at least as large as advertised
                for i,put in ipairs(write_kinds) do
                    put(core, addr, size, input, segment)
                    for _,get in ipairs(read_kinds) do
                        check_read(core, segment, addr, size, input, get)
                    end
                end
            end
        end
    end
end

