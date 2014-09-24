local class  = require 'middleclass'
local engine = require 'apoapsis.engine'
local DestroyCollisionShape = engine.DestroyCollisionShape


--- Defines the shape of a #Solid.
-- There are different types of collision shapes, each of them has its own
-- constructor function.
local CollisionShape = class('apoapsis/core/CollisionShape')

function CollisionShape:initialize( handle )
    self.handle = handle
end

function CollisionShape:destroy()
    DestroyCollisionShape(self.handle)
    self.handle = nil
end


return CollisionShape
