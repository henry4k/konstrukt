local class          = require 'middleclass'
local CollisionShape = require 'core/CollisionShape'
local CreateCompoundCollisionShape = ENGINE.CreateCompoundCollisionShape


local CompoundCollisionShape = class('core/collision_shapes/CompoundCollisionShape', CollisionShape)

function CompoundCollisionShape:initialize( ... )
    assert(false, 'Not yet implemented.') -- TODO
    CollisionShape.initialize(self, CreateCompoundCollisionShape(...))
end


return CompoundCollisionShape
