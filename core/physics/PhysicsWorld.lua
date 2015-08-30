--- @module core.physics.PhysicsWorld
--- Controls the global attributes of the physics simulation.


local assert     = assert
local Vec        = require 'core/Vector'
local SetGravity = ENGINE.SetGravity


local PhysicsWorld = {}

--- Changes the gravitational force which affects solids.
function PhysicsWorld.setGravity( force )
    assert(Vec:isInstance(force), 'Force must be vector.')
    SetGravity(force[1],
               force[2],
               force[3])
end


return PhysicsWorld
