local class = require 'core/middleclass.lua'
local CollisionShape = require 'core/CollisionShape.lua'


local CompoundCollisionShape = class('core/collision_shapes/CompoundCollisionShape', CollisionShape)

function CompoundCollisionShape:initialize( ... )
    -- TODO TODO TODO
    CollisionShape.initialize(self, NATIVE.CreateCompoundCollisionShape(...))
end


return CompoundCollisionShape
