local class = require 'middleclass'


local VoxelWriter = class('core/world/VoxelWriter')

function VoxelWriter:initialize( voxelVolume, structure )
    self.voxelVolume = voxelVolume
    self.structure   = structure
end

function VoxelWriter:writeVoxel( position, voxel )
    if self.structure:ownsVoxel(position) then
        return self.voxelVolume:writeVoxel(position, voxel)
    else
        error('Can\'t write voxel at '..tostring(position)..', as it doesn\'t belong to the structure.')
    end
end


return VoxelWriter
