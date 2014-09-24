local class          = require 'middleclass'
local CollisionShape = require 'apoapsis.core.CollisionShape'
local engine         = require 'apoapsis.engine'
local CreateCompoundCollisionShape = engine.CreateCompoundCollisionShape


local CompoundCollisionShape = class('apoapsis/core/collision_shapes/CompoundCollisionShape', CollisionShape)

function CompoundCollisionShape:initialize( ... )
    -- TODO TODO TODO
    CollisionShape.initialize(self, CreateCompoundCollisionShape(...))
end


return CompoundCollisionShape
