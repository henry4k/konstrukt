--- @classmod core.world.SingleVoxelStructure
--- Base class for structures which consist of a single voxel.
--
-- Extends @{core.world.Structure}.


local class         = require 'middleclass'
local Vec           = require 'core/Vector'
local Structure     = require 'core/world/Structure'
local Voxel         = require 'core/world/Voxel'
local VoxelAccessor = require 'core/world/VoxelAccessor'


local SingleVoxelStructure = class('core/world/SingleVoxelStructure', Structure)

function SingleVoxelStructure.static:getOrigin( voxel, position )
    return position
end

function SingleVoxelStructure:ownsVoxel( position )
    return position == self.origin
end


return SingleVoxelStructure
