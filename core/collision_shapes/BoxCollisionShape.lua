local class = require 'core/middleclass'
local Vec   = require 'core/Vector'
local CollisionShape = require 'core/CollisionShape'


local BoxCollisionShape = class('core/collision_shapes/BoxCollisionShape', CollisionShape)

function BoxCollisionShape:initialize( halfWidths )
    CollisionShape.initialize(self, NATIVE.CreateBoxCollisionShape(halfWidths:unpack(3)))
end


return BoxCollisionShape
