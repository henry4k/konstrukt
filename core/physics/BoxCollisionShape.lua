--- @classmod core.physics.BoxCollisionShape
--- Defines a cube for collision tests.
--
-- Extends @{CollisionShape}.


local assert         = assert
local class          = require 'middleclass'
local Vec            = require 'core/Vector'
local CollisionShape = require 'core/physics/CollisionShape'
local CreateBoxCollisionShape = ENGINE.CreateBoxCollisionShape


local BoxCollisionShape = class('core/physics/BoxCollisionShape', CollisionShape)

---
-- @param[type=Vector] halfWidths
-- Half widths are the radii of a box.
--
function BoxCollisionShape:initialize( halfWidths )
    assert(Vec:isInstance(halfWidths), 'Half width must be a vector.')
    CollisionShape.initialize(self, CreateBoxCollisionShape(halfWidths:unpack(3)))
end


return BoxCollisionShape
