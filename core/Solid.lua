local class = require 'core/middleclass.lua'


local Solid = class('core/Solid')

function Solid:initialize( collisionShape )
    self.handle = NATIVE.CreateSolid( collisionShape.handle )
end

function Solid:getPosition()
    NATIVE.GetSolidPosition(self.handle)
end

function Solid:getRotation()
    NATIVE.GetSolidRotation(self.handle)
end


return Solid
