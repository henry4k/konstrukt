--- @classmod core.voxel.UndefinedStructure
--- Placeholder for voxels which haven't been initialized yet.
--
-- Extends @{core.voxel.SingleVoxelStructure}.


local class                = require 'middleclass'
local SingleVoxelStructure = require 'core/voxel/SingleVoxelStructure'


local UndefinedStructure = class('core/voxel/UndefinedStructure', SingleVoxelStructure)

function UndefinedStructure:create()
    error('Undefined structures may not be created.')
end

function UndefinedStructure:read()
    -- Do nothing here.
end

function UndefinedStructure:write()
    error('Undefined structures may not be written.')
end


return UndefinedStructure
