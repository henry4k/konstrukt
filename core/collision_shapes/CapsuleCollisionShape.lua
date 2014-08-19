local class = require 'core/middleclass'
local CollisionShape = require 'core/CollisionShape'


local CapsuleCollisionShape = class('core/collision_shapes/CapsuleCollisionShape', CollisionShape)

function CapsuleCollisionShape:initialize( radius, height )
    CollisionShape.initialize(self, NATIVE.CreateCapsuleCollisionShape(radius, height))
end


return CapsuleCollisionShape
