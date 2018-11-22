local ffi = require("ffi")
local stem = "Model"

local impl = ffi.load("impl-" .. stem)
local blackbox = ffi.load("attinyxc")
ffi.cdef("const char * get_header_" .. stem .. "();");
ffi.cdef(ffi.string(impl.get_header_Model()))
ffi.cdef[[
int model_api_ver();
Model *model_ctor(const char *);
int model_dtor(Model *);
]]

local model = {}
model._ud  = blackbox.model_ctor("")
model._ver = blackbox.model_api_ver()
ffi.gc(model._ud, blackbox.model_dtor)

local list = { "reset", "cycle", "stop" }
for f in io.open("methods." .. stem .. ".txt"):lines() do
    model[f] = function(self, ...)
        return impl["Model__" .. f](self._ud, ...)
    end
end

model:stop()
