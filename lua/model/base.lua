--[[
    This file implements a bare-bones LuaJIT interface to the "Model"
    implemented in a libat*.so library.
--]]

local ffi = require("ffi")
local Proto = {}

Proto.Model = { __overrides = {} }
Proto.Core = { __overrides = {} }

local function define_c(ret, name, ...)
    return ffi.cdef(ret .. " " .. name .. "(" .. table.concat({...}, ",") .. ");")
end

local impl = ffi.load(arg[1])
arg = {select(2, unpack(arg))}
local get_h = "get_header_interface_hh_h_"
define_c("const char *", get_h)
ffi.cdef(ffi.string(impl[get_h]()))

for _, stem in ipairs({ "Model", "Core" }) do
    local proto = Proto[stem]
    local get_m = "get_methods_" .. stem .. "_txt_"
    define_c("const char *", get_m)

    -- create Lua entry points for C wrappers for C++ virtual calls
    for f in string.gmatch(ffi.string(impl[get_m]()), "%S+") do
        proto[f] = function(self, ...)
            return impl[stem .. "__" .. f](self, ...)
        end
    end

    local trapper = {
        __index    = function(_, k)    error("bad get for key='" .. k .. "'") end,
        __newindex = function(_, k, _) error("bad put for key='" .. k .. "'") end,
    }
    setmetatable(trapper, trapper)

    local handlers = {
        __index = function(ct, key)
            return proto[key] or
                (proto.__overrides[key] and proto.__overrides[key](ct)) or
                trapper[key]
        end,
        __newindex = proto,
    }
    ffi.metatype(stem, handlers)
end

define_c("int"    , "model_api_ver")
define_c("Model *", "model_ctor"   , "const char *")
define_c("void"   , "model_dtor"   , "Model *")

Proto.Model.create = function(_, libstem, name)
    local lib = ffi.load(libstem)
    local self = lib.model_ctor(name)
    self._lib = lib
    self._ver = lib.model_api_ver()
    return ffi.gc(self, self._lib.model_dtor)
end

return Proto

