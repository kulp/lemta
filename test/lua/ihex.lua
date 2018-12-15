function dehex(str)
    local result = ""
    for group in string.gfind(str, "%x%x") do
        result = result .. string.char(tonumber(group, 16))
    end
    return result
end

function read_ihex(lines,actor)
    for line in lines do
        local _, _, n, addr, kind, data, ck =
            string.find(line, "^:(%x%x)(%x%x%x%x)(%x%x)(%x*)(%x%x)%s*$")
        n    = tonumber(n   , 16)
        addr = tonumber(addr, 16)
        kind = tonumber(kind, 16)
        data = dehex(data)
        ck   = tonumber(ck  , 16)
        actor(n,addr,kind,data,ck)
    end
end

return { read = read_ihex }
