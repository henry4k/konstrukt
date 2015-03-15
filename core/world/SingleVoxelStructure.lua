local assert        = assert
local tostring      = tostring
local class         = require 'middleclass'
local Object        = class.Object
local Vec           = require 'core/Vector'
local WorldObject   = require 'core/world/WorldObject'
local Voxel         = require 'core/world/Voxel'
local VoxelVolume   = require 'core/world/VoxelVolume'
local VoxelAccessor = require 'core/world/VoxelAccessor'


local SingleVoxelStructure = class('core/world/SingleVoxelStructure', Structure)

local voxelAccessor = Structure.voxelAccessor -- cause we don't need own data fields

function SingleVoxelStructure:initialize( ... )
    Structure.initialize(self, ...)
end

function SingleVoxelStructure:getVoxelAt( position )
    return self.voxelVolume:getVoxelAt(position)
end

function SingleVoxelStructure:saveVoxelAt( position, voxel )
    assert(position == self.origin, 'Writes are only allowed at the origin.')
    return self.voxelVolume:saveVoxel(position, voxel)
end

function SingleVoxelStructure:clear()
    self:saveVoxelAt(self.origin, Voxel())
end

function SingleVoxelStructure:save()
    local id = self.class.id
    assert(id, 'SingleVoxelStructure has no id assigned yet.  Forgot to register it in the SingleVoxelStructure dictionary?')

    local originVoxel = Voxel()
    voxelAccessor:write(originVoxel, 'id', id)
end

function SingleVoxelStructure:read()
    -- Since the id doesn't need to be read again, this function serves
    -- just as a dummy.  It is meant to be overridden in child classes.
end

function SingleVoxelStructure:generateModels( chunkBuilder )
    -- Dummy function.  This is meant to be overridden in child classes.
end


return SingleVoxelStructure

