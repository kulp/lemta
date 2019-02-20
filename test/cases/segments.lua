local ffi = require("ffi")
local Model = require("model/sugared").Model
local Test = require("test")

local model = Model:create(unpack(arg))
local core = model:getCore(0)
local segment_names = { "SEG_PROG", "SEG_DATA", "SEG_IO" }

ffi.cdef[[int memcmp(const void *s1, const void *s2, size_t n);]]

function check_read(core, segment, addr, size, input, fashion, invert)
    local output = ffi.new("unsigned char[?]", size)
    for i = 0, size-1 do
        output[i] = 0xa5
    end

    fashion(core, addr, size, output, segment)

    local equal = ffi.C.memcmp(input, output, size) == 0
    local normal  = not invert and     equal
    local flipped =     invert and not equal
    Test.expect(true, normal or flipped)
end

local write_kinds = {
        function(c, a, s, m, p) c:writeMemory(a, s, m, p) end,
        function(c, a, s, m, p) c.segments[p].write(a, s, m) end,
        function(c, a, s, m, p) for i = 0, s-1 do c.segments[p].mem[a + i] = m[i] end end,
    }

local read_kinds = {
        function(c, a, s, m, p) c:readMemory(a, s, m, p) end,
        function(c, a, s, m, p) c.segments[p].read(a, s, m) end,
        function(c, a, s, m, p) for i = 0, s-1 do m[i] = c.segments[p].mem[a + i] end end,
    }

for _, segment_name in ipairs(segment_names) do
    local segment = ffi.cast("Segment", segment_name)
    -- integer property index N*2+5 appears to be the size of segment N
    local size = model.props[segment * 2 + 5].int()
    if size > 0 then
        local input = ffi.new("unsigned char[?]", size * 2)

        -- integer property index N*2+6 appears to be the base address of segment N
        local addr = model.props[segment * 2 + 6].int()

        for i = 0, size*2-1 do
            input[i] = 0xff - i
        end

        do
            local env = os.getenv("SKIP_UPPER_BOUND_CHECK_SEGMENT_" .. segment_name)
            if tonumber(env) ~= 1 then
                -- ensure segment is not larger than advertised
                for i, put in ipairs(write_kinds) do
                    put(core, addr, size + 1, input, segment)
                    for _, get in ipairs(read_kinds) do
                        check_read(core, segment, addr, size + 1, input, get, true)
                    end
                end
            end
        end

        do
            local env = os.getenv("SKIP_LOWER_BOUND_CHECK_SEGMENT_" .. segment_name)
            if tonumber(env) ~= 1 then
                -- ensure segment is at least as large as advertised
                for i, put in ipairs(write_kinds) do
                    put(core, addr, size, input, segment)
                    for _, get in ipairs(read_kinds) do
                        check_read(core, segment, addr, size, input, get)
                    end
                end
            end
        end
    end
end

