local ffi = require("ffi")
local Proto = {}

Proto.Core = { ["__overrides"] = {} }
Proto.Core.__overrides.regs =
    function(self)
        local regs = {}
        self.__proto.regs = regs
        regs.__index = function(_,index)
            local output = ffi.new("unsigned long[1]")
            self:peekReg(index, output)
            return output[0]
        end
        regs.__newindex = function(_,index,value)
            self:pokeReg(index, value)
        end

        setmetatable(regs, regs)
        return regs
    end

Proto.Core.__overrides.segments =
    function(self)
        local segments = {}
        self.__proto.segments = segments
        segments.__index = function(_,index)
            local obj = {}
            obj.read = function(addr, size, output)
                return self:readMemory(addr, size, output, index)
            end
            obj.write = function(addr, size, input)
                return self:writeMemory(addr, size, input, index)
            end
            return obj
        end

        setmetatable(segments, segments)
        return segments
    end

local function define_c(ret,name,...)
    return ffi.cdef(ret .. " " .. name .. "(" .. table.concat({...}, ",") .. ");")
end

for _,stem in ipairs({ "Model", "Core" }) do
    Proto[stem] = Proto[stem] or {}
    local proto = Proto[stem]
    local impl = ffi.load("impl-" .. stem)
    local get_h = "get_header_" .. stem .. "_h_"
    local get_m = "get_methods_" .. stem .. "_txt_"
    define_c("const char *", get_h)
    define_c("const char *", get_m)
    ffi.cdef(ffi.string(impl[get_h]()))

    -- create Lua entry points for C wrappers for C++ virtual calls
    for f in string.gmatch(ffi.string(impl[get_m]()), "%S+") do
        proto[f] = function(self, ...)
            return impl[stem .. "__" .. f](self, ...)
        end
    end

    proto.__proto = proto

    local handlers = {
        ["__index"] = function(ct,key)
            return proto[key] or proto.__overrides[key](ct)
        end,
        ["__newindex"] = proto,
    }
    ffi.metatype(stem, handlers)
end

define_c("int"    , "model_api_ver")
define_c("Model *", "model_ctor"   , "const char *")
define_c("int"    , "model_dtor"   , "Model *")

Proto.Model.create = function(proto,libstem,name)
    local lib = ffi.load(libstem)
    local self = lib.model_ctor(name)
    self._lib = lib
    self._ver = lib.model_api_ver()
    return ffi.gc(self,self._lib.model_dtor)
end

--[[
    TODO instead of registering a single thunk for each Lua callback for
    addCycleCallback and addStepCallback, register a single Lua callback that
    calls all others, to avoid expensively crossing the FFI boundary more than
    necessary. Preserve the ability to get the old behavior, though, in case
    some use-case requires the underlying library to control ordering of
    callback invocation.
--]]

-- wrap existing autogenerated addCycleCallback to drop userdata, since we can
-- rely on closures instead. If a clear need for an argument arises, the
-- argument can be restored (or _addCycleCallback can be used), but a manual
-- ffi.cast() will still be required to make most cases useful.
Proto.Model._addCycleCallback = Proto.Model.addCycleCallback
Proto.Model.addCycleCallback = function(self,cb)
    return self:_addCycleCallback(cb, nil)
end

-- wrap addStepCallback in the same way as Model:addCycleCallback
Proto.Core._addStepCallback = Proto.Core.addStepCallback
Proto.Core.addStepCallback = function(self,cb)
    return self:_addStepCallback(cb, nil)
end

return Proto.Model

