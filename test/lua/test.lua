local Test = {}

Test.dummy = function() end

Test.expect = function(expected, actual)

    local handle = error
    if expected == actual then
        handle = Test.dummy
    end

    handle("expected = " .. tostring(expected) .. ", actual = " .. tostring(actual))

end

return Test
