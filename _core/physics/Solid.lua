--- @classmod core.physics.Solid
--- A body that is simulated by the physics engine.
--
-- If a solid has no mass (i.e. it equals zero), it is concidered to be static.
-- So it isn't affected by collisions with other solids or gravity. Also each
-- solid needs a @{core.physics.CollisionShape}, but try to reuse
-- collision shapes to save memory.


local engine = require 'engine'
local class  = require 'middleclass'
local Object = class.Object
local Vec    = require 'core/Vector'
local Quat   = require 'core/Quaternion'
local Force  = require 'core/physics/Force'
local CollisionShape = require 'core/physics/CollisionShape'
local EventSource    = require 'core/EventSource'


local Solid = class('core/physics/Solid')
Solid:include(EventSource)


local SolidHandlesToSolids = {}
setmetatable(SolidHandlesToSolids, {__mode='k'})

--- Create a new solid.
-- @param mass
-- @param[type=core.Vector] position
-- @param[type=core.Quaternion] rotation
-- @param[type=core.physics.CollisionShape] collisionShape
--
function Solid:initialize( mass, position, rotation, collisionShape )
    assert(mass >= 0, 'Mass must be positive.')
    assert(Vec:isInstance(position), 'Position must be a vector.')
    assert(Object.isInstanceOf(rotation, Quat), 'Rotation must be a quaternion.')
    assert(Object.isInstanceOf(collisionShape, CollisionShape), 'Invalid collision shape.')
    self:initializeEventSource()
    self.handle = engine.CreateSolid(mass,
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
    self:destroyEventSource()
    for force,_ in pairs(self.forces) do
        force:destroy()
    end
    engine.DestroySolid(self.handle)
    self.handle = nil
end

--- Returns the solids mass or zero, if it's static.
function Solid:getMass()
    return engine.GetSolidMass(self.handle)
end

--- Changes the solids mass.
-- May be a positive value or zero.
-- Passing zero marks the object as static/immovable.
function Solid:setMass( mass )
    assert(mass >= 0, 'Mass must be positive.')
    engine.SetSolidMass(self.handle, mass)
end

--- Changes a solids restitution factor, which defines its 'bouncyness'.
-- @param[type=number] restitution
-- Between 0 and 1.
function Solid:setRestitution( restitution )
    assert(math.isBetween(restitution, 0, 1), 'Restitution must be between 0 and 1.')
    engine.SetSolidRestitution(self.handle, restitution)
end

--- Changes a solids friction.
function Solid:setFriction( friction )
    assert(friction >= 0, 'Friction must be positive.')
    engine.SetSolidFriction(self.handle, friction)
end

--- Only collisions with an impulse greater than `thresholdImpulse` will trigger the collision event.
-- Collision events are disabled by default.
function Solid:setCollisionThreshold( threshold )
    assert(threshold >= 0, 'Threshold must be positive.')
    engine.SetSolidCollisionThreshold(self.handle, threshold)
end

--- Prevents collision events from being triggered for this solid. (Which is the default behaviour.)
function Solid:disableCollisionEvents()
    engine.SetSolidCollisionThreshold(self.handle, -1)
end

--- Returns the solids current position as 3d vector.
function Solid:getPosition()
    return Vec(engine.GetSolidPosition(self.handle))
end

--- Returns the solids current orientation as quaternion.
function Solid:getRotation()
    return Quat(engine.GetSolidRotation(self.handle))
end

--- Velocity at which the solid moves through space.
-- @return[type=core.Vector]
function Solid:getLinearVelocity()
    return Vec(engine.GetSolidLinearVelocity(self.handle))
end

--- Velocity at which the solid rotates around itself.
-- @return[type=core.Vector]
function Solid:getAngularVelocity()
    return Vec(engine.GetSolidAngularVelocity(self.handle))
end

 --- Only solids which have gravity enabled will be affected by it.
 -- The gravity is enabled for all solids per default.
function Solid:enableGravity( enable )
    engine.EnableGravityForSolid(self.handle, enable)
end

--- Instantly applies an impulse.
--
-- In contrast to forces, impulses are independent of the simulation rate.
--
-- @param value
-- Describes the magnitude and direction.
--
-- @param relativePosition
-- Point where the impulse is applied to the solid.
-- Optional: defaults to 0,0,0
--
-- @param useLocalCoordinates
-- If set direction and position will be relative to the solids orientation.
--
function Solid:applyImpulse( value, relativePosition, useLocalCoordinates )
    assert(Vec:isInstance(value), 'Value must be a vector.')
    assert(Vec:isInstance(relativePosition) or
           relativePosition == nil,
           'Relative position must be a vector.')

    relativePosition    = relativePosition or Vec(0,0,0)
    useLocalCoordinates = useLocalCoordinates or false
    engine.ApplySolidImpulse(self.handle,
                             value[1],
                             value[2],
                             value[3],
                             relativePosition[1],
                             relativePosition[2],
                             relativePosition[3],
                             useLocalCoordinates)
end

--- Creates a new force, which affects this solid.
-- Initially all properties are zero, so that the force has no effect.
-- @return[type=core.physics.Force]
function Solid:createForce()
    local force = Force(engine.CreateForce(self.handle))
    self.forces[force] = force.handle
    return force
end

--- Fired when one solid collided with another.
-- @event collision
-- @param[type=number] impulse
-- @param[type=core.physics.Solid] other
-- @param[type=core.Vector] contactPoint
local function CollisionHandler( solidAHandle,
                                 solidBHandle,
                                 pointOnAX, pointOnAY, pointOnAZ,
                                 pointOnBX, pointOnBY, pointOnBZ,
                                 normalOnBX, normalOnBY, normalOnBZ,
                                 impulse )

    local pointOnA  = Vec(pointOnAX, pointOnAY, pointOnAZ)
    local pointOnB  = Vec(pointOnBX, pointOnBY, pointOnBZ)
    --local normalOnB = Vec(normalOnBX, normalOnBY, normalOnBZ)

    local solidA = SolidHandlesToSolids[solidAHandle]
    local solidB = SolidHandlesToSolids[solidBHandle]

    assert(solidA and solidB)

    print(string.format('%s and %s collide with %f N/s',
                        solidAHandle,
                        solidBHandle,
                        impulse))

    solidA:fireEvent('collision', impulse, solidB, pointOnA, pointOnB )
    solidB:fireEvent('collision', impulse, solidA, pointOnB, pointOnA )
end
engine.SetEventCallback('Collision', CollisionHandler)


return Solid
