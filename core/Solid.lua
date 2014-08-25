local class = require 'core/middleclass'
local Vec   = require 'core/Vector'
local Quat  = require 'core/Quaternion'


local Solid = class('core/Solid')

function Solid:initialize( mass, position, rotation, collisionShape )
    self.handle = NATIVE.CreateSolid(mass,
                                     position[1],
                                     position[2],
                                     position[3],
                                     rotation.handle,
                                     collisionShape.handle)
end

function Solid:getMass()
    return NATIVE.GetSolidMass(self.handle)
end

function Solid:setMass( mass )
    return NATIVE.SetSolidMass(self.handle, mass)
end

function Solid:setRestitution( restitution )
    return NATIVE.SetSolidRestitution(self.handle, restitution)
end

function Solid:setFriction( friction )
    return NATIVE.SetSolidFriction(self.handle, friction)
end

function Solid:getPosition()
    return Vec:new(NATIVE.GetSolidPosition(self.handle))
end

function Solid:getRotation()
    return Quat:new(NATIVE.GetSolidRotation(self.handle))
end


return Solid
