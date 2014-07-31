local class = require 'core/middleclass.lua'
local Vec   = require 'core/Vector.lua'
local CollisionShape = require 'core/CollisionShape.lua'


local BoxCollisionShape = class('core/collision_shapes/BoxCollisionShape', CollisionShape)

function BoxCollisionShape:initialize( halfWidths )
    CollisionShape.initialize(self, NATIVE.CreateBoxCollisionShape(halfWidths:unpack(3)))
end


return BoxCollisionShape
