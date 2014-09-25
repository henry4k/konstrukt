local class          = require 'middleclass'
local CollisionShape = require 'core/CollisionShape'
local CreateCapsuleCollisionShape = ENGINE.CreateCapsuleCollisionShape


local CapsuleCollisionShape = class('core/collision_shapes/CapsuleCollisionShape', CollisionShape)

function CapsuleCollisionShape:initialize( radius, height )
    CollisionShape.initialize(self, CreateCapsuleCollisionShape(radius, height))
end


return CapsuleCollisionShape
