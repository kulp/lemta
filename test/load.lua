local ffi = require("ffi")
local stem = "Model"

local impl = ffi.load("impl-" .. stem)
local blackbox = ffi.load("attinyxc")
ffi.cdef("const char * get_header_" .. stem .. "_h_();");
ffi.cdef("const char * get_methods_" .. stem .. "_txt_();");
ffi.cdef(ffi.string(impl.get_header_Model_h_()))
ffi.cdef[[
int model_api_ver();
Model *model_ctor(const char *);
int model_dtor(Model *);
]]

local model = {}
model._ud  = blackbox.model_ctor("")
model._ver = blackbox.model_api_ver()
ffi.gc(model._ud, blackbox.model_dtor)

for f in string.gmatch(ffi.string(impl.get_methods_Model_txt_()), "%S+") do
    model[f] = function(self, ...)
        return impl["Model__" .. f](self._ud, ...)
    end
end

model:stop()
