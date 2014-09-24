local class          = require 'middleclass'
local Vec            = require 'apoapsis.core.Vector'
local CollisionShape = require 'apoapsis.core.CollisionShape'
local engine         = require 'apoapsis.engine'
local CreateBoxCollisionShape = CreateBoxCollisionShape


local BoxCollisionShape = class('apoapsis/core/collision_shapes/BoxCollisionShape', CollisionShape)

function BoxCollisionShape:initialize( halfWidths )
    CollisionShape.initialize(self, CreateBoxCollisionShape(halfWidths:unpack(3)))
end


return BoxCollisionShape
