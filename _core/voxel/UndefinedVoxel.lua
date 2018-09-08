--- @classmod core.voxel.UndefinedStructure
--- Placeholder for voxels which haven't been initialized yet.
--
-- Extends @{core.voxel.Voxel}.


local class = require 'middleclass'
local Voxel = require 'core/voxel/Voxel'

return class('core/voxel/UndefinedVoxel', Voxel)
