--- @classmod core.physics.CapsuleCollisionShape
--- Defines a capsule for collision tests.
--
-- Extends @{core.physics.CollisionShape}.


local engine         = require 'engine'
local class          = require 'middleclass'
local Scheduler      = require 'core/Scheduler'
local CollisionShape = require 'core/physics/CollisionShape'


local CapsuleCollisionShape = class('core/physics/CapsuleCollisionShape', CollisionShape)

---
-- @param radius
-- @param height
--
function CapsuleCollisionShape:initialize( radius, height )
    CollisionShape.initialize(self, Scheduler.awaitCall(engine.CreateCapsuleCollisionShape, radius, height))
end


return CapsuleCollisionShape
