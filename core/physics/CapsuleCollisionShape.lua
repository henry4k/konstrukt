local class          = require 'middleclass'
local CollisionShape = require 'core/physics/CollisionShape'
local CreateCapsuleCollisionShape = ENGINE.CreateCapsuleCollisionShape


local CapsuleCollisionShape = class('core/physics/CapsuleCollisionShape', CollisionShape)

function CapsuleCollisionShape:initialize( radius, height )
    CollisionShape.initialize(self, CreateCapsuleCollisionShape(radius, height))
end


return CapsuleCollisionShape
