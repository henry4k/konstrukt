local assert    = assert
local unpack    = table.unpack
local insert    = table.insert
local class     = require 'middleclass'
local Vec       = require 'core/Vector'
local Voxel     = require 'core/world/Voxel'
local Structure = require 'core/world/Structure'
local StructureDictionary = require 'core/world/StructureDictionary'
local SetVoxelVolumeSize  = ENGINE.SetVoxelVolumeSize
local ReadVoxelData       = ENGINE.ReadVoxelData
local WriteVoxelData      = ENGINE.WriteVoxelData


local weakValueMT = { __mode = 'v' }


local VoxelVolume = class('core/world/VoxelVolume')

VoxelVolume.static._singletonExists = false

function VoxelVolume:initialize( size )
    assert(not VoxelVolume.static._singletonExists,
           'At the moment only one voxel volume is supported!')
    VoxelVolume.static._singletonExists = true

    assert(Vec:isInstance(size) and #size == 3,
           'Size must be passed as 3d vector.')
    SetVoxelVolumeSize(size:unpack(3))

    self.structureCache = setmetatable({}, weakValueMT)
end

function VoxelVolume:destroy()
    self:flushCaches()
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

    -- No need to flush the structure cache, since its values are weak.
    -- self.structureCache = setmetatable({}, weakValueMT)
end

function VoxelVolume:_getStructureOrigin( position )
    -- TODO: Structures larger than 1,1,1 are not supported yet.
    return position
end

function VoxelVolume:_readStructureAt( originPosition )
    local voxel = self:readVoxel(originPosition)
    local id = Structure.voxelAccessor:read('id', voxel)
    local structureClass = StructureDictionary.getClassFromId(id)
    local structure = structureClass(self, originPosition)
    structure:read()
    return structure
end

function VoxelVolume:getStructureAt( position )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    local originPosition = self:_getStructureOrigin(position)
    local hash = tostring(originPosition)
    local structureCache = self.structureCache
    local structure = structureCache[hash]
    if not structure then
        structure = self:_readStructureAt(originPosition)
        structureCache[hash] = structure
    end
    return structure
end

function VoxelVolume:getStructuresInAABB( min, max )
    assert(Vec:isInstance(min) and
           Vec:isInstance(max), 'Min and max must be vectors.')
    assert(min:componentsLesserOrEqualTo(max), 'Min must be smaller than max.')

    local uniqueStructures = {}

    for z = min[3], max[3] do
    for y = min[2], max[2] do
    for x = min[1], max[1] do
        local position = Vec(x, y, z)
        local structure = self:getStructureAt(position)
        uniqueStructures[structure] = true
    end
    end
    end

    local structures = {}
    for structure, _ in pairs(uniqueStructures) do
        insert(structures, structure)
    end
    return structures
end


return VoxelVolume
