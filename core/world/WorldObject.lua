--- Something in the game world, which you can see and interact with.
-- @module core.world.WorldObject


local class = require 'middleclass'
local EffectTarget = require 'core/EffectTarget'


local WorldObject = class('core/world/WorldObject')
WorldObject:include(EffectTarget)

function WorldObject:initialize()
    EffectTarget.initializeEffectTarget(self)
end

function WorldObject:destroy()
    EffectTarget.destroyEffectTarget(self)
end


return WorldObject
