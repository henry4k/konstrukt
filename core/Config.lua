local Config = {}

function Config.get( key, default )
    return NATIVE.GetConfigValue(key, default)
end

return Config
