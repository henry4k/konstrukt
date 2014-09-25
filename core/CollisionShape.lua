local class  = require 'middleclass'
local DestroyCollisionShape = ENGINE.DestroyCollisionShape


--- Defines the shape of a #Solid.
-- There are different types of collision shapes, each of them has its own
-- constructor function.
local CollisionShape = class('core/CollisionShape')

function CollisionShape:initialize( handle )
    self.handle = handle
end

function CollisionShape:destroy()
    DestroyCollisionShape(self.handle)
    self.handle = nil
end


return CollisionShape
