local class = require 'core/middleclass'


local CollisionShape = class('core/CollisionShape')

function CollisionShape:initialize( handle )
    self.handle = handle
end


return CollisionShape
