--- @classmod core.physics.BoxCollisionShape
--- Defines a cube for collision tests.
--
-- Extends @{core.physics.CollisionShape}.


local engine         = require 'engine'
local class          = require 'middleclass'
local Scheduler      = require 'core/Scheduler'
local Vec            = require 'core/Vector'
local CollisionShape = require 'core/physics/CollisionShape'


local BoxCollisionShape = class('core/physics/BoxCollisionShape', CollisionShape)

---
-- @param[type=core.Vector] halfWidths
-- Half widths are the radii of a box.
--
function BoxCollisionShape:initialize( halfWidths )
    assert(Vec:isInstance(halfWidths), 'Half width must be a vector.')
    CollisionShape.initialize(self, Scheduler.awaitCall(engine.CreateBoxCollisionShape, halfWidths:unpack(3)))
end


return BoxCollisionShape
