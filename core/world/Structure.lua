local assert        = assert
local tostring      = tostring
local class         = require 'middleclass'
local Object        = class.Object
local Vec           = require 'core/Vector'
local WorldObject   = require 'core/world/WorldObject'
local VoxelVolume   = require 'core/world/VoxelVolume'
local VoxelAccessor = require 'core/world/VoxelAccessor'


--- Static world objects, which are made from voxels use this as their base class.
-- Structures have an origin point and a size, wich defines an AABB.
-- This defines which voxel belong to the structure.
local Structure = class('core/world/Structure', WorldObject)

--- Helper function! So you don't need to import the StructureDictionary every time.
function Structure.static:register()
    local dict = require 'core/world/StructureDictionary'
    dict.registerClass(self)
end

local maxSize = Vec(15, 15, 15)

local voxelAccessor = VoxelAccessor()
voxelAccessor:addMask('id', 10)
voxelAccessor:addMask('size_x', 4)
voxelAccessor:addMask('size_y', 4)
voxelAccessor:addMask('size_z', 4)
Structure.static.voxelAccessor = voxelAccessor

function Structure:initialize( voxelVolume, origin )
    assert(Object.isInstanceOf(voxelVolume, VoxelVolume),
           'Must be called with a voxel volume.')
    assert(Vec:isInstance(origin), 'Origin must be a vector!')

    WorldObject.initialize(self)
    self.voxelVolume = voxelVolume
    self.origin = origin

    -- TODO: Read during initialization is bad!
    -- Separate loading and initialization!
    local originVoxel = voxelVolume:readVoxel(origin)
    self.size = Vec(voxelAccessor:read('size_x', originVoxel),
                    voxelAccessor:read('size_y', originVoxel),
                    voxelAccessor:read('size_z', originVoxel))
end

function Structure:destroy()
    WorldObject.destroy(self)
end

---
-- Other structures that are inside this new AABB are destroyed.
-- Raises an error if they are not completly contained.
--
-- ^- What if the structure is shrunk?
--
function Structure:setSize( size )
    assert(Vec:isInstance(size), 'Size must be a vector!')
    assert(size:componentsLesserOrEqualTo(maxSize), 'Size larger than maxSize!')
    self.size = size
    error('unimplemented') -- TODO!
end

function Structure:generateModels( chunkBuilder )
    -- Dummy function.  This is meant to be overridden in child classes.
end


return Structure
