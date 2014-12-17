--- Defines a sphere for collision tests.
--
-- Extends @{core.physics.CollisionShape}.
--
-- @classmod core.physics.SphereCollisionShape


local class          = require 'middleclass'
local CollisionShape = require 'core/physics/CollisionShape'
local CreateSphereCollisionShape = ENGINE.CreateSphereCollisionShape


local SphereCollisionShape = class('core/physics/SphereCollisionShape', CollisionShape)

---
-- @param radius
--
function SphereCollisionShape:initialize( radius )
    CollisionShape.initialize(self, CreateSphereCollisionShape(radius))
end


return SphereCollisionShape
