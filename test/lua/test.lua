local Test = {}

local function equal(a, b)
    return a == b
end

local function not_equal(a, b)
    return not equal(a, b)
end

Test.dummy = function() end

Test.want = function(accepter, compare, actual)
    local handle = error
    if accepter(compare, actual) then
        handle = Test.dummy
    end

    handle("compare = " .. tostring(compare) .. ", actual = " .. tostring(actual))
end

Test.expect = function(expected, actual)
    return Test.want(equal, expected, actual)
end

Test.expect_not = function(not_expected, actual)
    return Test.want(not_equal, not_expected, actual)
end

return Test
