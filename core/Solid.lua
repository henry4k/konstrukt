local class = require 'core/middleclass.lua'
local Vec   = require 'core/Vector.lua'
local Quat  = require 'core/Quaternion.lua'


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

function Solid:getPosition()
    return Vec:new(NATIVE.GetSolidPosition(self.handle))
end

function Solid:getRotation()
    return Quat:new(NATIVE.GetSolidRotation(self.handle))
end


return Solid
