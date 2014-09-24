local class          = require 'middleclass'
local CollisionShape = require 'apoapsis.core.CollisionShape'
local engine         = require 'apoapsis.engine'
local CreateCapsuleCollisionShape = engine.CreateCapsuleCollisionShape


local CapsuleCollisionShape = class('apoapsis/core/collision_shapes/CapsuleCollisionShape', CollisionShape)

function CapsuleCollisionShape:initialize( radius, height )
    CollisionShape.initialize(self, CreateCapsuleCollisionShape(radius, height))
end


return CapsuleCollisionShape
