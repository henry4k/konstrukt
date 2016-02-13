local engine      = require 'engine'
local class       = require 'middleclass'
local Object      = class.Object
local EventSource = require 'core/EventSource'
local Vec         = require 'core/Vector'
local VoxelData   = require 'core/voxel/VoxelData'
local Voxel       = require 'core/voxel/Voxel'
local VoxelDictionary = require 'core/voxel/VoxelDictionary'


local weakValueMT = { __mode = 'v' }


local VoxelVolume = class('core/voxel/VoxelVolume')
VoxelVolume:include(EventSource)

function VoxelVolume:initialize( size )
    assert(Vec:isInstance(size) and #size == 3,
           'Size must be passed as 3d vector.')
    self.handle = engine.CreateVoxelVolume(size:unpack(3))

    self:initializeEventSource()

    self.voxelCache = setmetatable({}, weakValueMT)
end

function VoxelVolume:destroy()
    self:destroyEventSource()
    engine.DestroyVoxelVolume(self.handle)
    self.handle = nil
end

--- Returns @{core.voxel.VoxelData} or `nil` if something went wrong.
function VoxelVolume:_readVoxelData( position )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    local voxelData = engine.ReadVoxelData(self.handle, position:unpack(3))
    if voxelData then
        return VoxelData(voxelData)
    end
end

--- Fired when a voxel has been modified.
-- @event voxel-modified
-- @param[type=core.Vector] position

--- Returns whether the operation was successfull.
function VoxelVolume:_writeVoxelData( position, voxelData )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    assert(VoxelData:isInstance(voxelData), 'Must be called with a voxel data object.')
    local result = engine.WriteVoxelData(self.handle, position[1], position[2], position[3], voxelData)
    if result then
        self:fireEvent('voxel-modified', position)
    end
    return result
end

--- Creates a new voxel at the given position.
function VoxelVolume:createVoxelAt( position, voxelClass, _voxelData )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    assert(Object.isSubclassOf(voxelClass, Voxel), 'Voxel class must inherit from the voxel base class.')
    assert(voxelClass.id,
           'The voxels class has no ID.  Forgot to register it in the VoxelDictionary?  Or hasn\'t assignIds() been called yet?')

    local voxel = voxelClass:allocate()
    voxel._voxelData = _voxelData or VoxelData()
    voxel:setAttribute('id', voxelClass.id)
    voxel:initialize()

    --local positionHash = tostring(position)
    --self.voxelCache[positionHash] = voxel

    -- TODO: Check whether position is writable.
    -- TODO: VoxelEntity gets its position here.

    return voxel
end

--- Write voxel to position.
-- Returns whether the operation was successfull.
function VoxelVolume:setVoxelAt( position, voxel )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    assert(Object.isInstanceOf(voxel, Voxel),
           'Voxel must be a class inheriting voxel base class.')
    -- TODO: Copy voxel entity if entity.voxelPosition ~= position.
    local success = self:_writeVoxelData(position, voxel._voxelData)
    if success then
        local positionHash = tostring(position)
        self.voxelCache[positionHash] = voxel
    end
    return success
end

--- Returns @{core.voxel.Voxel} or `nil` if something went wrong.
function VoxelVolume:getVoxelAt( position )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    local positionHash = tostring(position)
    local voxel = self.voxelCache[positionHash]
    if not voxel then
        local voxelData = self:_readVoxelData(position)
        if voxelData then
            local voxelClassId = voxelData:read(0, Voxel.static.idBitCount)
            local voxelClass = VoxelDictionary.getClassFromId(voxelClassId)
            voxel = self:createVoxelAt(position, voxelClass, voxelData)
        end
    end
    return voxel
end

function VoxelVolume:getVoxelsInAABB( min, max )
    assert(Vec:isInstance(min) and
           Vec:isInstance(max), 'Min and max must be vectors.')
    assert(min:componentsLesserOrEqualTo(max), 'Min must be smaller than max.')

    local voxels = {}

    for z = min[3], max[3] do
    for y = min[2], max[2] do
    for x = min[1], max[1] do
        local position = Vec(x, y, z)
        local voxel = self:getVoxelAt(position)
        if voxel then
            table.insert(voxels, voxel)
        end
    end
    end
    end

    return voxels
end


return VoxelVolume
