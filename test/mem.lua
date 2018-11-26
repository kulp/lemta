local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

-- raw read/write interface
local size = 3
local input  = ffi.new("unsigned char[?]", size, { 9, 8, 7 })
local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
local segment = 0
core:writeMemory(0x102, size, input, segment)
core:readMemory( 0x102, size, output, segment)

ffi.cdef[[int memcmp(const void *s1, const void *s2, size_t n);]]

local handle = error
if ffi.C.memcmp(input,output,size) == 0 then
    handle = print
end

handle("input = " .. tostring(input) .. ", output = " .. tostring(output))

