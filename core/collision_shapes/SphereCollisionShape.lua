local class = require 'core/middleclass'
local CollisionShape = require 'core/CollisionShape'


local SphereCollisionShape = class('core/collision_shapes/SphereCollisionShape', CollisionShape)

function SphereCollisionShape:initialize( radius )
    CollisionShape.initialize(self, NATIVE.CreateSphereCollisionShape(radius))
end


return SphereCollisionShape
