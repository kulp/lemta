local ffi = require("ffi")
local Model = require("model/sugared").Model

local lib, chip = unpack(arg)
local model = Model:create(lib, chip)
local core = model:getCore(0)
local segment_names = { "SEG_PROG", "SEG_DATA", "SEG_EEPROM", "SEG_FUSES" }

for _, segment_name in ipairs(segment_names) do
    local segment = ffi.cast("Segment", segment_name)
    -- integer property index N*2+5 appears to be the size of segment N
    local size = model.props[segment * 2 + 5].int()
    local chip = model.props[0].string()
    local _, _, name = string.find(lib, "lib(at%w+).so")
    local t = { name, chip, segment_name, "SIZE" }
    print(table.concat(t, "_") .. "=" .. size)
end

