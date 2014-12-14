--- Can be used to change the behaviour of an object at runtime.
--
-- To be able to host effects, objects need to include the @{EffectTarget} mixin.
--
-- @classmod core.Effect


local class  = require 'middleclass'
local Object = class.Object


local Effect = class('core/Effect')

--- The priority determines in which order effects receive callbacks.
--
-- Long lasting effects should have a low priority, while
-- short lasting effects should have a high priority.
--
-- The default value is zero, which should be seen as medium priority.
--
Effect.static.priority = 0

--- Is called by @{EffectTarget:addEffect}.
function Effect:initialize( target )
    self.target = target
    --self.priority = 0
end

--- Is called by @{EffectTarget:removeEffect}.
function Effect:destroy()
end

--- Compare effects by their priority.
function Effect:__lt( other )
    assert(Object.instanceOf(other, Effect), 'Can compare only Effect instances.')
    return self.prioriy < other.priority
end


return Effect
