local m = {}

function m.get( key, default )
    return NATIVE.GetConfigValue(key, default)
end

return m
