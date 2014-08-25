local class = require 'core/middleclass'
local Vec   = require 'core/Vector'
local Quat  = require 'core/Quaternion'


--- A body that is simulated by the physics engine.
-- If a solid has no mass (i.e. it equals zero), it is concidered to be static.
-- So it isn't affected by collisions with other solids or gravity. Also each
-- solid needs a #CollisionShape, but try to reuse collision shapes to save
-- memory.
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

--- Changes a solids restitution factor, which defines its 'bouncyness'.
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

--- Velocity at which the solid moves through space.
function Solid:getLinearVelocity()
    return Vec:new(NATIVE.GetSolidLinearVelocity(self.handle))
end

--- Velocity at which the solid rotates around itself.
function Solid:getAngularVelocity()
    return Vec:new(NATIVE.GetSolidAngularVelocity(self.handle))
end


return Solid
