--- @classmod core.physics.EmptyCollisionShape
--- A collision shape without actual collision detection.
--
-- Extends @{core.physics.CollisionShape}.


local engine         = require 'engine'
local class          = require 'middleclass'
local Scheduler      = require 'core/Scheduler'
local CollisionShape = require 'core/physics/CollisionShape'


local EmptyCollisionShape = class('core/physics/EmptyCollisionShape', CollisionShape)

function EmptyCollisionShape:initialize()
    CollisionShape.initialize(self, Scheduler.awaitCall(engine.CreateEmptyCollisionShape))
end


return EmptyCollisionShape
