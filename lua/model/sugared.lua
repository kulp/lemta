--[[
    This file implements a "sugared" LuaJIT interface to the "Model"
    implemented in a libat*.so library, with ergonomic features added.
--]]

local ffi = require("ffi")
local Proto = require("model/base")
local ihex = require("ihex")

Proto.Core.__overrides.regs =
    function(self)
        local regs = {}
        regs.__index = function(_, index)
            local output = ffi.new("unsigned long[1]")
            if self:peekReg(ffi.cast("enum RegisterSpecial", index), output) ~= -1 then
                return tonumber(output[0])
            else
                return nil
            end
        end
        regs.__newindex = function(_, index, value)
            self:pokeReg(ffi.cast("enum RegisterSpecial", index), value)
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
        props.__index = function(_, index)
            local obj = {}
            obj.string = function(_)
                local size = 1024 -- TODO shrink meaningfully
                local output = ffi.new("char[?]", size)
                if self:getStringProperty(index, size, output, nil) ~= -1 then
                    return ffi.string(output)
                else
                    return "<nil>"
                end
            end
            obj.int = function(_)
                local output = ffi.new("unsigned long[1]")
                if self:getIntProperty(index, output, nil) ~= -1 then
                    return tonumber(output[0])
                else
                    return -1
                end
            end
            obj.__tostring = obj.string
            setmetatable(obj, obj)

            return obj
        end
        props.__newindex = function(_, index, value)
            if type(value) == "number" then
                return self:setIntProperty(index, value, nil)
            else
                -- We assume pessimistically that setStringProperty may
                -- modify its argument, although this is unlikely
                local str = tostring(value)
                return self:setStringProperty(index, ffi.new("char[?]", str:len() + 1, str), nil)
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
        segments.__index = function(_, index)
            local obj = {}
            obj.read = function(addr, size, output)
                return self:readMemory(addr, size, output, index)
            end
            obj.write = function(addr, size, input)
                return self:writeMemory(addr, size, input, index)
            end

            local mem = {}
            mem.__index = function(_, addr)
                local temp = ffi.new("unsigned char[1]")
                if obj.read(addr, 1, temp) ~= -1 then
                    return tonumber(temp[0])
                else
                    return nil
                end
            end
            mem.__newindex = function(_, addr, value)
                obj.write(addr, 1, ffi.new("unsigned char[1]", value))
            end
            setmetatable(mem, mem)
            obj.mem = mem
            setmetatable(obj, mem) -- allow .segments[x][y] as well as .segments[x].mem[y]

            return obj
        end

        setmetatable(segments, segments)
        return segments
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
Proto.Model.addCycleCallback = function(self, cb)
    return self:_addCycleCallback(cb, nil)
end

-- wrap addStepCallback in the same way as Model:addCycleCallback
Proto.Core._addStepCallback = Proto.Core.addStepCallback
Proto.Core.addStepCallback = function(self, cb)
    return self:_addStepCallback(cb, nil)
end

-- for now, create a zeroed breakpoint taking no arguments
Proto.Core.createBreakpoint = function()
    return ffi.new("Breakpoint")
end

Proto.Core.cycles = function(self, cyc)
    local old = self.regs["REG_CYCLES"]
    if cyc ~= nil then
        self.regs["REG_CYCLES"] = cyc
    end
    return old
end

Proto.Core.load = function(self, hexfile)
    local prog = self.segments["SEG_PROG"]

    local function loader(n, addr, kind, data, ck)
        return kind == 0 and prog.write(addr, n, data)
    end

    ihex.read(io.lines(hexfile), loader)
end

return Proto
