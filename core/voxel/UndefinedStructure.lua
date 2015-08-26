--- @classmod core.voxel.UndefinedStructure
--- Placeholder for voxels which haven't been initialized yet.
--
-- Extends @{core.voxel.SingleVoxelStructure}.


local class                = require 'middleclass'
local SingleVoxelStructure = require 'core/voxel/SingleVoxelStructure'


local UndefinedStructure = class('core/voxel/UndefinedStructure', SingleVoxelStructure)

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
