local class          = require 'middleclass'
local CollisionShape = require 'core/CollisionShape'
local CreateSphereCollisionShape = ENGINE.CreateSphereCollisionShape


local SphereCollisionShape = class('core/collision_shapes/SphereCollisionShape', CollisionShape)

function SphereCollisionShape:initialize( radius )
    CollisionShape.initialize(self, CreateSphereCollisionShape(radius))
end


return SphereCollisionShape
