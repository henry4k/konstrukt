local class = require 'middleclass'
local Vec   = require 'core/Vector'
local Voxel = require 'core/voxel/Voxel'
local SetVoxelVolumeSize = ENGINE.SetVoxelVolumeSize
local ReadVoxelData      = ENGINE.ReadVoxelData
local WriteVoxelData     = ENGINE.WriteVoxelData


local VoxelVolume = class('core/voxel/VoxelVolume')

VoxelVolume.static._singletonExists = false

function VoxelVolume:initialize( size )
    assert(not VoxelVolume.static._singletonExists,
           'At the moment only one voxel volume is supported!')
    VoxelVolume.static._singletonExists = true

    assert(Vec:isInstance(size) and #size == 3,
           'Size must be passed as 3d vector.')
    SetVoxelVolumeSize(size:unpack(3))
end

function VoxelVolume:destroy()
end

function VoxelVolume:readVoxel( position )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    local data = ReadVoxelData(position:unpack(3))
    return Voxel(table.unpack(data))
end

function VoxelVolume:writeVoxel( position, voxel )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    assert(Voxel:isInstance(voxel), 'Must be called with a voxel.')
    WriteVoxelData(position[1], position[2], position[3], voxel)
end


return VoxelVolume
