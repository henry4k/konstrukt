local class = require 'core/middleclass'
local CollisionShape = require 'core/CollisionShape'


local CompoundCollisionShape = class('core/collision_shapes/CompoundCollisionShape', CollisionShape)

function CompoundCollisionShape:initialize( ... )
    -- TODO TODO TODO
    CollisionShape.initialize(self, NATIVE.CreateCompoundCollisionShape(...))
end


return CompoundCollisionShape
