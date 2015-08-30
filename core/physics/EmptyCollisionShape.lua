--- @classmod core.physics.EmptyCollisionShape
--- A collision shape without actual collision detection.
--
-- Extends @{core.physics.CollisionShape}.


local class          = require 'middleclass'
local CollisionShape = require 'core/physics/CollisionShape'
local CreateEmptyCollisionShape = ENGINE.CreateEmptyCollisionShape


local EmptyCollisionShape = class('core/physics/EmptyCollisionShape', CollisionShape)

function EmptyCollisionShape:initialize()
    CollisionShape.initialize(self, CreateEmptyCollisionShape())
end


return EmptyCollisionShape
