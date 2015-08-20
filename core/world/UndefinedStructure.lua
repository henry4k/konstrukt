--- @classmod core.world.UndefinedStructure
--- Placeholder for voxels which haven't been initialized yet.
--
-- Extends @{core.world.SingleVoxelStructure}.


local class                = require 'middleclass'
local SingleVoxelStructure = require 'core/world/SingleVoxelStructure'


local UndefinedStructure = class('core/world/UndefinedStructure', SingleVoxelStructure)

function UndefinedStructure:initialize( ... )
    SingleVoxelStructure.initialize(self, ...)
end

function UndefinedStructure:destroy()
    SingleVoxelStructure.destroy()
end

function UndefinedStructure:create( voxelCreator )
    error('Undefined structures may not be created.')
end

function UndefinedStructure:read( voxelReader )
    -- Do nothing here.
end

function UndefinedStructure:write( voxelWriter )
    error('Undefined structures may not be written.')
end


return UndefinedStructure
