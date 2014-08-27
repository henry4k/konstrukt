local class = require 'core/middleclass'
local Vec   = require 'core/Vector'
local Quat  = require 'core/Quaternion'
local Force = require 'core/Force'


--- A body that is simulated by the physics engine.
-- If a solid has no mass (i.e. it equals zero), it is concidered to be static.
-- So it isn't affected by collisions with other solids or gravity. Also each
-- solid needs a #CollisionShape, but try to reuse collision shapes to save
-- memory.
local Solid = class('core/Solid')

local SolidHandlesToSolids = {}
setmetatable(SolidHandlesToSolids, {__mode='k'})

function Solid:initialize( mass, position, rotation, collisionShape )
    self.handle = NATIVE.CreateSolid(mass,
                                     position[1],
                                     position[2],
                                     position[3],
                                     rotation.handle,
                                     collisionShape.handle)
    self.forces = {}
    setmetatable(self.forces, {__mode='v'})

    SolidHandlesToSolids[self.handle] = self
end

function Solid:destroy()
    for force,_ in pairs(self.forces) do
        force:destroy()
    end
    self.handle = nil
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

--- Only collisions with an impulse greater than `thresholdImpulse` will trigger the collision event.
-- The default threshold is `math.huge`.
function Solid:setCollisionThreshold( threshold )
    return NATIVE.SetSolidCollisionThreshold(self.handle, threshold)
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

--- Instantly applies an impulse.
-- In contrast to forces, impulses are independent of the simulation rate.
--
-- @param impulseVector
-- Describes the magnitude and direction.
--
-- @param relativePosition
-- Point where the impulse is applied to the solid.
-- Optional: defaults to 0,0,0
--
-- @param useLocalCoordinates
-- If set direction and position will be relative to the solids orientation.
function Solid:applyImpulse( impulseVector, relativePosition, useLocalCoordinates )
    relativePosition = relativePosition or Vec:new(0,0,0)
    NATIVE.ApplySolidImpulse(self.handle,
                             impulseVector[1],
                             impulseVector[2],
                             impulseVector[3],
                             relativePosition[1],
                             relativePosition[2],
                             relativePosition[3],
                             useLocalCoordinates)
end

function Solid:createForce()
    local force = Force:new(self.handle)
    self.forces[force] = force.handle
    return force
end


local function CollisionHandler( solidAHandle,
                                 solidBHandle,
                                 pointOnAX, pointOnAY, pointOnAZ,
                                 pointOnBX, pointOnBY, pointOnBZ,
                                 normalOnBX, normalOnBY, normalOnBZ,
                                 impulse )

    print(string.format('collision with %f N/s', impulse))

    --local pointOnA  = Vec:new(pointOnAX, pointOnAY, pointOnAZ)
    --local pointOnB  = Vec:new(pointOnBX, pointOnBY, pointOnBZ)
    --local normalOnB = Vec:new(normalOnBX, normalOnBY, normalOnBZ)
    --
    --local solidA = SolidHandlesToSolids[solidAHandle]
    --local solidB = SolidHandlesToSolids[solidBHandle]
    --
    --print(string.format('%s and %s collide with %f N/s',
    --                    solidAHandle,
    --                    solidBHandle,
    --                    impulse))
end
NATIVE.SetEventCallback('Collision', CollisionHandler)


return Solid
