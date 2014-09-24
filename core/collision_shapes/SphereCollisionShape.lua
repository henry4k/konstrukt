local class          = require 'middleclass'
local CollisionShape = require 'apoapsis.core.CollisionShape'
local engine         = require 'apoapsis.engine'
local CreateSphereCollisionShape = engine.CreateSphereCollisionShape


local SphereCollisionShape = class('apoapsis/core/collision_shapes/SphereCollisionShape', CollisionShape)

function SphereCollisionShape:initialize( radius )
    CollisionShape.initialize(self, CreateSphereCollisionShape(radius))
end


return SphereCollisionShape
