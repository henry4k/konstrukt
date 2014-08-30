local class = require 'core/middleclass'


--- Defines the shape of a #Solid.
-- There are different types of collision shapes, each of them has its own
-- constructor function.
local CollisionShape = class('core/CollisionShape')

function CollisionShape:initialize( handle )
    self.handle = handle
end

function CollisionShape:destroy()
    NATIVE.DestroyCollisionShape(self.handle)
    self.handle = nil
end


return CollisionShape
