local ffi = require("ffi")
local Model = require("model")

local model = Model:create(unpack(arg))
local core = model:getCore(0)

ffi.cdef[[int memcmp(const void *s1, const void *s2, size_t n);]]

-- raw read/write interface
local size = 3
local addr = 0x102
local segment = 0
local input = ffi.new("unsigned char[?]", size, { 9, 8, 7 })
core:writeMemory(addr, size, input, segment)

do
    local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
    core:readMemory( addr, size, output, segment)

    local handle = error
    if ffi.C.memcmp(input,output,size) == 0 then
        handle = print
    end

    handle("input = " .. tostring(input) .. ", output = " .. tostring(output))
end

do
    local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
    core.segments[segment].read(addr, size, output)

    local handle = error
    if ffi.C.memcmp(input,output,size) == 0 then
        handle = print
    end

    handle("input = " .. tostring(input) .. ", output = " .. tostring(output))
end

do
    local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
    for i = 0,size-1 do
        output[i] = core.segments[segment].mem[addr + i]
    end

    local handle = error
    if ffi.C.memcmp(input,output,size) == 0 then
        handle = print
    end

    handle("input = " .. tostring(input) .. ", output = " .. tostring(output))
end

addr = addr - 10
core.segments[segment].write(addr, size, input)

do
    local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
    core:readMemory( addr, size, output, segment)

    local handle = error
    if ffi.C.memcmp(input,output,size) == 0 then
        handle = print
    end

    handle("input = " .. tostring(input) .. ", output = " .. tostring(output))
end

do
    local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
    core.segments[segment].read(addr, size, output)

    local handle = error
    if ffi.C.memcmp(input,output,size) == 0 then
        handle = print
    end

    handle("input = " .. tostring(input) .. ", output = " .. tostring(output))
end

do
    local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
    for i = 0,size-1 do
        output[i] = core.segments[segment].mem[addr + i]
    end

    local handle = error
    if ffi.C.memcmp(input,output,size) == 0 then
        handle = print
    end

    handle("input = " .. tostring(input) .. ", output = " .. tostring(output))
end

addr = addr - 20
for i = 0,size-1 do
    core.segments[segment].mem[addr + i] = input[i]
end

do
    local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
    core:readMemory( addr, size, output, segment)

    local handle = error
    if ffi.C.memcmp(input,output,size) == 0 then
        handle = print
    end

    handle("input = " .. tostring(input) .. ", output = " .. tostring(output))
end

do
    local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
    core.segments[segment].read(addr, size, output)

    local handle = error
    if ffi.C.memcmp(input,output,size) == 0 then
        handle = print
    end

    handle("input = " .. tostring(input) .. ", output = " .. tostring(output))
end

do
    local output = ffi.new("unsigned char[?]", size, { 0, 0, 0 })
    for i = 0,size-1 do
        output[i] = core.segments[segment].mem[addr + i]
    end

    local handle = error
    if ffi.C.memcmp(input,output,size) == 0 then
        handle = print
    end

    handle("input = " .. tostring(input) .. ", output = " .. tostring(output))
end

