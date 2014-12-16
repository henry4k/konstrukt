--- Defines a collision shape by joining multiple other collision shapes.
--
-- *Still unimplemented!*
--
-- Extends @{CollisionShape}.
--
-- @classmod core.physics.CompoundCollisionShape


local class          = require 'middleclass'
local CollisionShape = require 'core/physics/CollisionShape'
local CreateCompoundCollisionShape = ENGINE.CreateCompoundCollisionShape


local CompoundCollisionShape = class('core/physics/CompoundCollisionShape', CollisionShape)

function CompoundCollisionShape:initialize( ... )
    assert(false, 'Not yet implemented.') -- TODO
    CollisionShape.initialize(self, CreateCompoundCollisionShape(...))
end


return CompoundCollisionShape
