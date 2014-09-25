local class          = require 'middleclass'
local CollisionShape = require 'core/CollisionShape'
local CreateCompoundCollisionShape = ENGINE.CreateCompoundCollisionShape


local CompoundCollisionShape = class('core/collision_shapes/CompoundCollisionShape', CollisionShape)

function CompoundCollisionShape:initialize( ... )
    -- TODO TODO TODO
    CollisionShape.initialize(self, CreateCompoundCollisionShape(...))
end


return CompoundCollisionShape
