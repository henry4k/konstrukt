--- Defines the shape of a @{Solid}.
-- There are different types of collision shapes, each of them has its own
-- constructor function.
--
-- @module core.physics.CollisionShape


local class  = require 'middleclass'
local DestroyCollisionShape = ENGINE.DestroyCollisionShape


local CollisionShape = class('core/physics/CollisionShape')

function CollisionShape:initialize( handle )
    self.handle = handle
end

function CollisionShape:destroy()
    DestroyCollisionShape(self.handle)
    self.handle = nil
end


return CollisionShape
