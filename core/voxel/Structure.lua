--- @classmod core.voxel.Structure
--- Static world objects, which are made from voxels use this as their base class.
--
-- Extends @{core.world.WorldObject}.


local class         = require 'middleclass'
local WorldObject   = require 'core/world/WorldObject'
local VoxelAccessor = require 'core/voxel/VoxelAccessor'
local VoxelCreator  = require 'core/voxel/VoxelCreator'
local VoxelReader   = require 'core/voxel/VoxelReader'
local VoxelWriter   = require 'core/voxel/VoxelWriter'


local Structure = class('core/voxel/Structure', WorldObject)

--- Helper function! So you don't need to import the StructureDictionary every time.
function Structure.static:register()
    local dict = require 'core/voxel/StructureDictionary'
    dict.registerClass(self)
end

local voxelAccessor = VoxelAccessor()
voxelAccessor:addMask('id', 10)
Structure.static.voxelAccessor = voxelAccessor

function Structure.static:getOrigin( voxel, position )
    error('Implementation missing.')
end

function Structure:initialize()
    WorldObject.initialize(self)
    self.origin = nil
    self.structureAccessor = nil -- TODO: Implement StructureAccessor!
end

function Structure:destroy()
    WorldObject.destroy(self)
end

function Structure:_create( voxelVolume, origin, ... )
    self.origin = origin
    local voxelCreator = VoxelCreator(voxelVolume, self)
    self:create(voxelCreator, ...)
end

function Structure:_read( voxelVolume, origin )
    self.origin = origin
    local voxelReader = VoxelReader(voxelVolume, self)
    assert(voxelReader)
    self:read(voxelReader)
end

function Structure:_write( voxelVolume )
    local voxelWriter = VoxelWriter(voxelVolume, self)
    self:write(voxelWriter)
end

function Structure:ownsVoxel( position )
    error('Implementation missing.')
end

function Structure:create( voxelCreator )
    -- Dummy function.  This is meant to be overridden in child classes.
end

function Structure:read( voxelReader )
    -- Dummy function.  This is meant to be overridden in child classes.
end

function Structure:write( voxelWriter )
    -- Dummy function.  This is meant to be overridden in child classes.
end

function Structure:generateModels( chunkBuilder )
    -- Dummy function.  This is meant to be overridden in child classes.
end


return Structure
