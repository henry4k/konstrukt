---
-- @module core.Config


local GetConfigValue = ENGINE.GetConfigValue


local Config = {}

---
-- @param key
-- @param[opt=nil] default
function Config.get( key, default )
    return GetConfigValue(key, default)
end

return Config
