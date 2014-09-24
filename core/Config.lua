local engine = require 'apoapsis.engine'
local GetConfigValue = engine.GetConfigValue


local Config = {}

function Config.get( key, default )
    return GetConfigValue(key, default)
end

return Config
