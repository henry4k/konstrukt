--- @classmod core.world.Structure
--- Static world objects, which are made from voxels use this as their base class.
--
-- Extends @{WorldObject}.


local class = require 'middleclass'
local WorldObject = require 'core/world/WorldObject'


local Structure = class('core/world/Structure', WorldObject)

function Structure:initialize()
    WorldObject.initialize(self)
end

function Structure:destroy()
    WorldObject.destroy(self)
end


return Structure
