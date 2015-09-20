local class       = require 'middleclass'
local VoxelWriter = require 'core/voxel/VoxelWriter'


local VoxelCreator = class('core/voxel/VoxelCreator', VoxelWriter)

function VoxelCreator:initialize( ... )
    VoxelWriter.initialize(self, ...)
end

function VoxelCreator:writeVoxel( position, voxel )
    local structure = self.structure
    if structure:ownsVoxel(position) then
        local voxelVolume = self.voxelVolume
        local oldStructure = voxelVolume:getStructureAt(position)
        if oldStructure and oldStructure ~= structure then
            -- TODO: Inform structure, which intersects the new area, about its destruction.
            print(string.format('TODO: Replaced structure %s should be destroyed.', oldStructure.class.name))
        end
        if not voxelVolume:writeVoxel(position, voxel) then
            error('Can\'t create voxel at '..tostring(position)..'.')
        end
    else
        error('Can\'t create voxel at '..tostring(position)..', as it doesn\'t belong to the structure.')
    end
end


return VoxelCreator
