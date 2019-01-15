local lib = {}

function lib.null_terminated(list)
    local i = 0
    return function()
        local result = list[i]
        if result ~= nil then
            i = i + 1
            return result
        end
    end
end

function lib.null_terminated_length(list)
    local count = 0
    for v in lib.null_terminated(list) do
        count = count + 1
    end
    return count
end

return lib

