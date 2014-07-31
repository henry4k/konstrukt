local class = require 'core/middleclass.lua'
local CollisionShape = require 'core/CollisionShape.lua'


local CapsuleCollisionShape = class('core/collision_shapes/CapsuleCollisionShape', CollisionShape)

function CapsuleCollisionShape:initialize( radius, height )
    CollisionShape.initialize(self, NATIVE.CreateCapsuleCollisionShape(radius, height))
end


return CapsuleCollisionShape
