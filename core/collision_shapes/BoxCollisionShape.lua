local class          = require 'middleclass'
local Vec            = require 'core/Vector'
local CollisionShape = require 'core/CollisionShape'
local CreateBoxCollisionShape = CreateBoxCollisionShape


local BoxCollisionShape = class('core/collision_shapes/BoxCollisionShape', CollisionShape)

function BoxCollisionShape:initialize( halfWidths )
    CollisionShape.initialize(self, CreateBoxCollisionShape(halfWidths:unpack(3)))
end


return BoxCollisionShape
