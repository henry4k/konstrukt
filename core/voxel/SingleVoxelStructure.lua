--- @classmod core.voxel.SingleVoxelStructure
--- Base class for structures which consist of a single voxel.
--
-- Extends @{core.voxel.Structure}.


local class     = require 'middleclass'
local Structure = require 'core/voxel/Structure'


local SingleVoxelStructure = class('core/voxel/SingleVoxelStructure', Structure)

function SingleVoxelStructure.static:getOrigin( voxel, position )
    return position
end

function SingleVoxelStructure:ownsVoxel( position )
    return position == self.origin
end


return SingleVoxelStructure
