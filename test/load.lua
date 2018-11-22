local ffi = require("ffi")
local stem = "Model"

local model = ffi.load("impl-" .. stem)
local blackbox = ffi.load("attinyxc")

io.input("header." .. stem .. ".h")

ffi.cdef(io.read("*a"))
ffi.cdef[[
int model_api_ver();
Model *model_ctor(const char *);
int model_dtor(Model *);
]]

local ver = blackbox.model_api_ver()
print("ver = "..ver)
local m = blackbox.model_ctor("")
model.Model__stop(m)
