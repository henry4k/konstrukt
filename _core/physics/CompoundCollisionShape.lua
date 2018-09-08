--- @classmod core.physics.CompoundCollisionShape
--- Defines a collision shape by joining multiple other collision shapes.
--
-- *Still unimplemented!*
--
-- Extends @{core.physics.CollisionShape}.


local engine         = require 'engine'
local class          = require 'middleclass'
local CollisionShape = require 'core/physics/CollisionShape'


local CompoundCollisionShape = class('core/physics/CompoundCollisionShape', CollisionShape)

function CompoundCollisionShape:initialize( ... )
    assert(false, 'Not yet implemented.') -- TODO
    CollisionShape.initialize(self, engine.CreateCompoundCollisionShape(...))
end


return CompoundCollisionShape
