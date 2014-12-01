local class       = require 'middleclass'
local WorldObject = require 'core/world/WorldObject'


--- Static world objects, which are made from voxels use this as their base class.
local Structure = class('core/world/Structure', WorldObject)

--- Helper function! So you don't need to import the StructureDictionary every time.
function Structure.static:register()
    local dict = require 'core/world/StructureDictionary'
    dict:registerClass(self)
end

function Structure:initialize( voxelVolume, origin )
    WorldObject.initialize(self)
end

function Structure:destroy()
    WorldObject.destroy(self)
end

function Structure:generateModels( chunkBuilder )
    -- Dummy function.  This is meant to be overridden in child classes.
end


return Structure
