local class = require 'middleclass'


local VoxelReader = class('core/world/VoxelReader')

function VoxelReader:initialize( voxelVolume, structure )
    self.voxelVolume = voxelVolume
    self.structure   = structure
end

function VoxelReader:readVoxel( position )
    if self.structure:ownsVoxel(position) then
        return self.voxelVolume:readVoxel(position)
    else
        error('Can\'t read voxel at '..position..', as it doesn\'t belong to the structure.')
    end
end


return VoxelReader
