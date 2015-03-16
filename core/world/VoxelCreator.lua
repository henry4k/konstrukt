local class       = require 'middleclass'
local VoxelWriter = require 'core/world/VoxelWriter'


local VoxelCreator = class('core/world/VoxelCreator', VoxelWriter)

function VoxelCreator:initialize( ... )
    VoxelWriter.initialize(self, ...)
end

function VoxelCreator:writeVoxel( position, voxel )
    local structure = self.structure
    if structure:ownsVoxel(position) then
        local voxelVolume = self.voxelVolume
        local oldStructure = voxelVolume:getStructure(position)
        if oldStructure ~= structure then
            -- TODO: Inform structure, which intersects the new area, about its destruction.
            print(oldStructure..' should be destroyed.')
        end
        return voxelVolume:writeVoxel(position, voxel)
    else
        error('Can\'t create voxel at '..position..', as it doesn\'t belong to the structure.')
    end
end


return VoxelCreator
