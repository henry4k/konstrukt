local class  = require 'middleclass'
local Vec    = require 'apoapsis.core.Vector'
local Quat   = require 'apoapsis.core.Quaternion'
local Force  = require 'apoapsis.core.Force'
local engine = require 'apoapsis.engine'
local CreateSolid                = engine.CreateSolid
local DestroySolid               = engine.DestroySolid
local GetSolidMass               = engine.GetSolidMass
local SetSolidMass               = engine.SetSolidMass
local SetSolidRestitution        = engine.SetSolidRestitution
local SetSolidFriction           = engine.SetSolidFriction
local SetSolidCollisionThreshold = engine.SetSolidCollisionThreshold
local GetSolidPosition           = engine.GetSolidPosition
local GetSolidRotation           = engine.GetSolidRotation
local GetSolidLinearVelocity     = engine.GetSolidLinearVelocity
local GetSolidAngularVelocity    = engine.GetSolidAngularVelocity
local ApplySolidImpulse          = engine.ApplySolidImpulse
local SetEventCallback           = engine.SetEventCallback


--- A body that is simulated by the physics engine.
-- If a solid has no mass (i.e. it equals zero), it is concidered to be static.
-- So it isn't affected by collisions with other solids or gravity. Also each
-- solid needs a #CollisionShape, but try to reuse collision shapes to save
-- memory.
local Solid = class('apoapsis/core/Solid')

local SolidHandlesToSolids = {}
setmetatable(SolidHandlesToSolids, {__mode='k'})

function Solid:initialize( mass, position, rotation, collisionShape )
    self.handle = CreateSolid(mass,
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
    DestroySolid(self.handle)
    self.handle = nil
end

function Solid:getMass()
    return GetSolidMass(self.handle)
end

function Solid:setMass( mass )
    return SetSolidMass(self.handle, mass)
end

--- Changes a solids restitution factor, which defines its 'bouncyness'.
function Solid:setRestitution( restitution )
    return SetSolidRestitution(self.handle, restitution)
end

function Solid:setFriction( friction )
    return SetSolidFriction(self.handle, friction)
end

--- Only collisions with an impulse greater than `thresholdImpulse` will trigger the collision event.
-- The default threshold is `math.huge`.
function Solid:setCollisionThreshold( threshold )
    return SetSolidCollisionThreshold(self.handle, threshold)
end

function Solid:getPosition()
    return Vec(GetSolidPosition(self.handle))
end

function Solid:getRotation()
    return Quat(GetSolidRotation(self.handle))
end

--- Velocity at which the solid moves through space.
function Solid:getLinearVelocity()
    return Vec(GetSolidLinearVelocity(self.handle))
end

--- Velocity at which the solid rotates around itself.
function Solid:getAngularVelocity()
    return Vec(GetSolidAngularVelocity(self.handle))
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
    relativePosition = relativePosition or Vec(0,0,0)
    ApplySolidImpulse(self.handle,
                      impulseVector[1],
                      impulseVector[2],
                      impulseVector[3],
                      relativePosition[1],
                      relativePosition[2],
                      relativePosition[3],
                      useLocalCoordinates)
end

function Solid:createForce()
    local force = Force(self.handle)
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

    --local pointOnA  = Vec(pointOnAX, pointOnAY, pointOnAZ)
    --local pointOnB  = Vec(pointOnBX, pointOnBY, pointOnBZ)
    --local normalOnB = Vec(normalOnBX, normalOnBY, normalOnBZ)
    --
    --local solidA = SolidHandlesToSolids[solidAHandle]
    --local solidB = SolidHandlesToSolids[solidBHandle]
    --
    --print(string.format('%s and %s collide with %f N/s',
    --                    solidAHandle,
    --                    solidBHandle,
    --                    impulse))
end
SetEventCallback('Collision', CollisionHandler)


return Solid
