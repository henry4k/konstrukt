local GetConfigValue = ENGINE.GetConfigValue


local Config = {}

function Config.get( key, default )
    return GetConfigValue(key, default)
end

return Config
