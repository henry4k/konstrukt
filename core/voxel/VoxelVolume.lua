local engine      = require 'engine'
local class       = require 'middleclass'
local Object      = class.Object
local EventSource = require 'core/EventSource'
local Vec         = require 'core/Vector'
local Voxel       = require 'core/voxel/Voxel'
local Structure   = require 'core/voxel/Structure'
local StructureDictionary = require 'core/voxel/StructureDictionary'


local weakValueMT = { __mode = 'v' }


local VoxelVolume = class('core/voxel/VoxelVolume')
VoxelVolume:include(EventSource)

function VoxelVolume:initialize( size )
    assert(Vec:isInstance(size) and #size == 3,
           'Size must be passed as 3d vector.')
    self.handle = engine.CreateVoxelVolume(size:unpack(3))

    self:initializeEventSource()

    self.structureCache = setmetatable({}, weakValueMT)
end

function VoxelVolume:destroy()
    self:destroyEventSource()
    engine.DestroyVoxelVolume(self.handle)
    self.handle = nil
end

--- Returns @{core.voxel.Voxel} or `nil` if something went wrong.
function VoxelVolume:readVoxel( position )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    local voxelData = engine.ReadVoxelData(self.handle, position:unpack(3))
    if voxelData then
        return Voxel(voxelData)
    end
end

--- Fired when a voxel has been modified.
-- @event voxel-modified
-- @param[type=core.Vector] position

--- Returns whether the operation was successfull.
function VoxelVolume:writeVoxel( position, voxel )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    assert(Voxel:isInstance(voxel), 'Must be called with a voxel.')
    local result = engine.WriteVoxelData(self.handle, position[1], position[2], position[3], voxel)
    if result then
        self:fireEvent('voxel-modified', position)
    end
    return result
end

function VoxelVolume:getStructureAt( position )
    assert(Vec:isInstance(position), 'Position must be a vector.')

    local positionHash = tostring(position)
    local structureCache = self.structureCache
    local structure = structureCache[positionHash]

    if not structure then
        local voxel = self:readVoxel(position)
        if not voxel then
            return
        end

        local id = Structure.voxelAccessor:read(voxel, 'id')
        -- TODO: Optimize voxelAccessor as a local?
        local structureClass = StructureDictionary.getClassFromId(id)

        local origin = structureClass:getOrigin(voxel, position)

        structure = structureClass()
        structure:_read(self, origin)

        structureCache[positionHash] = structure
        if origin ~= position then
            local originHash = tostring(origin)
            structureCache[originHash] = structure
        end
    end

    return structure
end

function VoxelVolume:createStructure( structureClass, origin, ... )
    assert(Object.isSubclassOf(structureClass, Structure),
           'Structure class must inherit the structure base class.')
    assert(structureClass.id,
           'Structure class has no ID.  Forgot to register it in the StructureDictionary?  Or hasn\'t assignIds() been called yet?')

    local structure = structureClass()
    structure:_create(self, origin, ...)

    local originHash = tostring(origin)
    self.structureCache[originHash] = structure

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
        if structure then
            uniqueStructures[structure] = true
        end
    end
    end
    end

    local structures = {}
    for structure, _ in pairs(uniqueStructures) do
        table.insert(structures, structure)
    end
    return structures
end


return VoxelVolume

