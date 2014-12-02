local assert    = assert
local unpack    = table.unpack
local class     = require 'middleclass'
local Vec       = require 'core/Vector'
local Voxel     = require 'core/world/Voxel'
local Structure = require 'core/world/Structure'
local StructureDictionary = require 'core/world/StructureDictionary'
local SetVoxelVolumeSize  = ENGINE.SetVoxelVolumeSize
local ReadVoxelData       = ENGINE.ReadVoxelData
local WriteVoxelData      = ENGINE.WriteVoxelData


local VoxelVolume = class('core/world/VoxelVolume')

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
    return Voxel(unpack(data))
end

function VoxelVolume:writeVoxel( position, voxel )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    assert(Voxel:isInstance(voxel), 'Must be called with a voxel.')
    WriteVoxelData(position[1], position[2], position[3], voxel)
end

function VoxelVolume:flushCaches()
    -- Dummy function.
end

function VoxelVolume:getStructureAt( position )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    local voxel = self:readVoxel(position)
    local id = Structure.voxelAccessor:read('id', voxel)
    local structureClass = StructureDictionary.getClassFromId(id)
    local structure = structureClass(self, position)
    return structure
end

function VoxelVolume:getStructuresInAABB( min, max )
    assert(Vec:isInstance(min) and
           Vec:isInstance(max), 'Min and max must be vectors.')
    assert(min:componentsLesserOrEqualTo(max), 'Min must be smaller than max.')
    error('unimplemented')
end


return VoxelVolume
