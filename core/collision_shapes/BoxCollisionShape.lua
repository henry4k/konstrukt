local assert         = assert
local class          = require 'middleclass'
local Vec            = require 'core/Vector'
local CollisionShape = require 'core/CollisionShape'
local CreateBoxCollisionShape = ENGINE.CreateBoxCollisionShape


local BoxCollisionShape = class('core/collision_shapes/BoxCollisionShape', CollisionShape)

function BoxCollisionShape:initialize( halfWidths )
    assert(Vec:isInstance(halfWidths), 'Half width must be a vector.')
    CollisionShape.initialize(self, CreateBoxCollisionShape(halfWidths:unpack(3)))
end


return BoxCollisionShape
