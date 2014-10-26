local class = require 'middleclass'
local EffectTarget = require 'core/EffectTarget'


--- Something in the game world, which you can see and interact with.
local WorldObject = class('core/world/WorldObject')
WorldObject:include(EffectTarget)

function WorldObject:initialize()
    EffectTarget.initializeEffectTarget(self)
end

function WorldObject:destroy()
    EffectTarget.destroyEffectTarget(self)
end


return WorldObject
