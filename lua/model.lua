local ffi = require("ffi")
local Proto = {}

Proto.Model = { __overrides = {} }

Proto.Core = { __overrides = {} }
Proto.Core.__overrides.regs =
    function(self)
        local regs = {}
        regs.__index = function(_,index)
            local output = ffi.new("unsigned long[1]")
            self:peekReg(index, output)
            return tonumber(output[0])
        end
        regs.__newindex = function(_,index,value)
            self:pokeReg(index, value)
        end
        regs.len = function()
            return self.props[1028].int() -- TODO hoist magic property identifier
        end
        regs.__len = regs.len

        setmetatable(regs, regs)
        return regs
    end

Proto.Core.__overrides.props =
    function(self)
        local props = {}
        props.__index = function(_,index)
            local obj = {}
            obj.string = function(_)
                local size = 1024 -- TODO shrink meaningfully
                local str = ffi.new("char[?]", size)
                self:getStringProperty(index, size, str, nil)
                return ffi.string(str)
            end
            obj.int = function(_)
                local var = ffi.new("unsigned long[1]")
                self:getIntProperty(index, var, nil)
                return tonumber(var[0])
            end
            obj.__tostring = obj.string
            setmetatable(obj,obj)

            return obj
        end
        props.__newindex = function(_,index,value)
            if type(value) == "number" then
                return self:setIntProperty(index, value, nil)
            else
                -- We assume optimistically that setStringProperty does not
                -- actually modify its argument
                return self:setStringProperty(index, ffi.cast("char *", tostring(value)), nil)
            end
        end

        setmetatable(props, props)
        return props
    end
-- Model has props too, with same access semantics
Proto.Model.__overrides.props = Proto.Core.__overrides.props

Proto.Core.__overrides.segments =
    function(self)
        local segments = {}
        segments.__index = function(_,index)
            local obj = {}
            obj.read = function(addr, size, output)
                return self:readMemory(addr, size, output, index)
            end
            obj.write = function(addr, size, input)
                return self:writeMemory(addr, size, input, index)
            end

            local mem = {}
            mem.__index = function(table, addr)
                local temp = ffi.new("unsigned char[1]")
                obj.read(addr, 1, temp)
                return tonumber(temp[0])
            end
            mem.__newindex = function(table, addr, value)
                obj.write(addr, 1, ffi.new("unsigned char[1]", value))
            end
            setmetatable(mem, mem)
            obj.mem = mem

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

    local trapper = {
        __index    = function(_,k)   error("bad get for key='" .. k .. "'") end,
        __newindex = function(_,k,v) error("bad put for key='" .. k .. "'") end,
    }
    setmetatable(trapper, trapper)

    local handlers = {
        ["__index"] = function(ct,key)
            return proto[key] or
                (proto.__overrides[key] and proto.__overrides[key](ct)) or
                trapper
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

