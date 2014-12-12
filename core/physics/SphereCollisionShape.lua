---
-- @module core.physics.SphereCollisionShape
-- @see CollisionShape


local class          = require 'middleclass'
local CollisionShape = require 'core/physics/CollisionShape'
local CreateSphereCollisionShape = ENGINE.CreateSphereCollisionShape


local SphereCollisionShape = class('core/physics/SphereCollisionShape', CollisionShape)

function SphereCollisionShape:initialize( radius )
    CollisionShape.initialize(self, CreateSphereCollisionShape(radius))
end


return SphereCollisionShape
