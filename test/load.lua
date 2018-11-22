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
end

local blackbox = ffi.load("attinyxc")
ffi.cdef[[
int model_api_ver();
Model *model_ctor(const char *);
int model_dtor(Model *);
]]

local model = Proto.Model
model._ud  = blackbox.model_ctor("")
model._ver = blackbox.model_api_ver()
ffi.gc(model._ud, blackbox.model_dtor)

model:stop()
