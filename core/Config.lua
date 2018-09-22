--- @module core.Config
--- Read configuration values.


local engine = require 'engine'
local Scheduler = require 'core/Scheduler'


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
    return Scheduler.awaitCall(engine.GetConfigValue, key, default)
end

return Config
