--- @classmod core.physics.SphereCollisionShape
--- Defines a sphere for collision tests.
--
-- Extends @{core.physics.CollisionShape}.


local engine         = require 'engine'
local class          = require 'middleclass'
local CollisionShape = require 'core/physics/CollisionShape'


local SphereCollisionShape = class('core/physics/SphereCollisionShape', CollisionShape)

---
-- @param radius
--
function SphereCollisionShape:initialize( radius )
    CollisionShape.initialize(self, engine.CreateSphereCollisionShape(radius))
end


return SphereCollisionShape
