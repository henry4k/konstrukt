local assert        = assert
local tostring      = tostring
local class         = require 'middleclass'
local Object        = class.Object
local Vec           = require 'core/Vector'
local WorldObject   = require 'core/world/WorldObject'
local Voxel         = require 'core/world/Voxel'
local VoxelVolume   = require 'core/world/VoxelVolume'
local VoxelAccessor = require 'core/world/VoxelAccessor'


--- Static world objects, which are made from voxels, use this as their base class.
local Structure = class('core/world/Structure', WorldObject)

--- Helper function! So you don't need to import the StructureDictionary every time.
function Structure.static:register()
    local dict = require 'core/world/StructureDictionary'
    dict.registerClass(self)
end

local voxelAccessor = VoxelAccessor()
voxelAccessor:addMask('id', 10)
Structure.static.voxelAccessor = voxelAccessor

--[[
    assert(Object.isInstanceOf(voxelVolume, VoxelVolume),
           'Must be called with a voxel volume.')
    assert(Vec:isInstance(origin), 'Origin must be a vector!')
  ]]

function Structure:initialize()
    WorldObject.initialize(self)
    self.origin = nil
    self.structureAccessor = nil -- is this needed?
end

function Structure:destroy()
    WorldObject.destroy(self)
end

function Structure:_read( voxelVolume, origin )
    local voxelReader = nil -- TODO
    self.origin = origin
    self:read(voxelReader)
    voxelReader:destroy()
end

function Structure:_create( voxelVolume, origin, ... ) -- TODO
    local voxelCreator = nil -- TODO
    self.origin = origin
    self:create(voxelCreator)
    voxelCreator:destroy()
    -- TODO: Inform structures, which intersect the new area, about their destruction.
end

function Structure:_write( voxelVolume )
    local voxelWriter = nil -- TODO
    self.origin = origin
    self:write(voxelWriter)
    voxelWriter:destroy()
end

function Structure:ownsVoxel( position )
    error('Implementation missing.')
end

function Structure:read( voxelReader )
    -- Dummy function.  This is meant to be overridden in child classes.
end

function Structure:create( voxelCreator )
    -- Dummy function.  This is meant to be overridden in child classes.
end

function Structure:write( voxelWriter )
    -- Dummy function.  This is meant to be overridden in child classes.
end

function Structure:generateModels( chunkBuilder )
    -- Dummy function.  This is meant to be overridden in child classes.
end


return Structure
