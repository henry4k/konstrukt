local class = require 'core/middleclass.lua'


local CollisionShape = class('core/CollisionShape')

function CollisionShape:initialize( handle )
    self.handle = handle
end


return CollisionShape
