---
--
-- @module core.Effect
-- @see EffectTarget


local class  = require 'middleclass'
local Object = class.Object


local Effect = class('core/Effect')

-- Long lasting effects should have a low priority
-- Short lasting effects should have a high priority
Effect.static.priority = 0

function Effect:initialize( target )
    self.target = target
    --self.priority = 0
end

function Effect:destroy() end

function Effect:__lt( other )
    assert(Object.instanceOf(other, Effect), 'Can compare only Effect instances.')
    return self.prioriy < other.priority
end


return Effect
