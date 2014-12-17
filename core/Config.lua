--- Read configuration values.
-- @module core.Config


local GetConfigValue = ENGINE.GetConfigValue


local Config = {}

--- Retrieve a configuration value.
--
-- @param key
--
-- @param[opt=nil] default
-- Used as a fallback value and to determine the type.
--
-- @return
-- The keys value or the default value, if the key was not configurated.
--
function Config.get( key, default )
    return GetConfigValue(key, default)
end

return Config
