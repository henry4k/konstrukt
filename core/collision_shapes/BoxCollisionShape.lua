local class = require 'core/middleclass.lua'
local CollisionShape = require 'core/CollisionShape.lua'


local BoxCollisionShape = class('core/collision_shapes/BoxCollisionShape', CollisionShape)

function BoxCollisionShape:initialize( w, h, d )
    CollisionShape.initialize(self, NATIVE.CreateBoxCollisionShape(w,h,d))
end


return BoxCollisionShape
