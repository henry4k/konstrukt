--- Defines the shape of a @{core.physics.Solid}.
--
-- There are different types of collision shapes, each of them has its own
-- constructor function.
--
-- @classmod core.physics.CollisionShape


local class  = require 'middleclass'
local DestroyCollisionShape = ENGINE.DestroyCollisionShape


local CollisionShape = class('core/physics/CollisionShape')

--- Abstract class.
--
-- Use @{core.physics.BoxCollisionShape}, @{core.physics.SphereCollisionShape},
-- @{core.physics.CapsuleCollisionShape}, or @{core.physics.CompoundCollisionShape}
-- instead.
--
function CollisionShape:initialize( handle )
    assert(self.class ~= CollisionShape, 'CollisionShape is an abstract class and not meant to be instanciated directly.')
    self.handle = handle
end

function CollisionShape:destroy()
    DestroyCollisionShape(self.handle)
    self.handle = nil
end


return CollisionShape
