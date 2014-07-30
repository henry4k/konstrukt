local class = require 'core/middleclass.lua'


local Solid = class('core/Solid')

function Solid:initialize( mass, position, rotation, collisionShape )
    self.handle = NATIVE.CreateSolid( collisionShape.handle )
end

function Solid:getMass()
    return NATIVE.GetSolidMass(self.handle)
end

function Solid:setMass( mass )
    return NATIVE.SetSolidMass(self.handle, mass)
end

function Solid:getPosition()
    return NATIVE.GetSolidPosition(self.handle)
end

function Solid:getRotation()
    return NATIVE.GetSolidRotation(self.handle)
end


return Solid
