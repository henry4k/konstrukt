--- @classmod core.world.Structure
--- Static world objects, which are made from voxels use this as their base class.
--
-- Extends @{core.world.WorldObject}.


local assert        = assert
local tostring      = tostring
local class         = require 'middleclass'
local Object        = class.Object
local Vec           = require 'core/Vector'
local WorldObject   = require 'core/world/WorldObject'
local Voxel         = require 'core/world/Voxel'
local VoxelVolume   = require 'core/world/VoxelVolume'
local VoxelAccessor = require 'core/world/VoxelAccessor'
local VoxelCreator  = require 'core/world/VoxelCreator'
local VoxelReader   = require 'core/world/VoxelReader'
local VoxelWriter   = require 'core/world/VoxelWriter'


local Structure = class('core/world/Structure', WorldObject)

--- Helper function! So you don't need to import the StructureDictionary every time.
function Structure.static:register()
    local dict = require 'core/world/StructureDictionary'
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
    self.structureAccessor = nil -- is this needed?
end

function Structure:destroy()
    WorldObject.destroy(self)
end

function Structure:_create( voxelVolume, origin, ... )
    self.origin = origin
    local voxelCreator = VoxelCreator(voxelVolume, self)
    self:create(voxelCreator, ...)
    voxelCreator:destroy()
end

function Structure:_read( voxelVolume, origin )
    self.origin = origin
    local voxelReader = VoxelReader(voxelVolume, self)
    self:read(voxelReader)
    voxelReader:destroy()
end

function Structure:_write( voxelVolume )
    self.origin = origin
    local voxelWriter = VoxelWriter(voxelVolume, self)
    self:write(voxelWriter)
    voxelWriter:destroy()
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
