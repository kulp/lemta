local ffi = require("ffi")
local Proto = {}

for _,stem in ipairs({ "Model", "Core" }) do
    Proto[stem] = {}
    local proto = Proto[stem]
    local impl = ffi.load("impl-" .. stem)
    local get_h = "get_header_" .. stem .. "_h_"
    local get_m = "get_methods_" .. stem .. "_txt_"
    ffi.cdef("const char * " .. get_h .. "();");
    ffi.cdef("const char * " .. get_m .. "();");
    ffi.cdef(ffi.string(impl[get_h]()))

    -- create Lua entry points for C wrappers for C++ virtual calls
    for f in string.gmatch(ffi.string(impl[get_m]()), "%S+") do
        proto[f] = function(self, ...)
            return impl[stem .. "__" .. f](self._ud, ...)
        end
    end

    proto._clone = function(self)
        local copy = {}
        for k,v in pairs(self) do
            copy[k] = v
        end
        return copy
    end

    proto._wrap = function(self,cdata)
        local self = self:_clone()
        self._ud = cdata
        return self
    end
end

ffi.cdef[[
int model_api_ver();
Model *model_ctor(const char *);
int model_dtor(Model *);
]]

Proto.Model.create = function(proto,libstem,name)
    local lib = ffi.load(libstem)
    local self = proto:_wrap(lib.model_ctor(name))
    self._lib = lib
    self._ver = lib.model_api_ver()
    return self
end

Proto.Model.destroy = function(self)
    local lib = self._lib
    lib.model_dtor(self._ud)
end

local function reg_lookup(regs,...)
    local self = regs._parent
    local arg = {...}
    local index = arg[1]
    if #arg == 1 then
        local output = ffi.new("unsigned long[1]")
        self:peekReg(index, output)
        return output[0]
    else
        local value = arg[2]
        self:pokeReg(index, value)
    end
end

local core_meta = {
    ["__index"] = reg_lookup,
    ["__newindex"] = reg_lookup,
}

-- wrap existing autogenerated getCore in order to translate into Lua objects
Proto.Model._getCore = Proto.Model.getCore
Proto.Model.getCore = function(self,...)
    local orig = self:_getCore(...)
    local self = Proto.Core:_wrap(orig)

    local regs = { ["_parent"] = self }
    setmetatable(regs, core_meta)
    self.regs = regs

    return self
end

-- wrap existing autogenerated addCycleCallback to drop userdata, since we can
-- rely on closures instead. If a clear need for an argument arises, the
-- argument can be restored (or _addCycleCallback can be used), but a manual
-- ffi.cast() will still be required to make most cases useful.
Proto.Model._addCycleCallback = Proto.Model.addCycleCallback
Proto.Model.addCycleCallback = function(self,cb)
    return self:_addCycleCallback(cb, nil)
end

-- wrap existing autogenerated getModel in order to translate into Lua objects
Proto.Core._getModel = Proto.Core.getModel
Proto.Core.getModel = function(self)
    local orig = self:_getModel()
    return Proto.Model:_wrap(orig)
end

return Proto.Model
