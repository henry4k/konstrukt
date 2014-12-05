local assert        = assert
local tostring      = tostring
local class         = require 'middleclass'
local Object        = class.Object
local Vec           = require 'core/Vector'
local WorldObject   = require 'core/world/WorldObject'
local Voxel         = require 'core/world/Voxel'
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

local minSize = Vec(1, 1, 1)
local maxSize = Vec(15, 15, 15) -- 2^4 - 1

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
    self.size = nil
end

function Structure:destroy()
    WorldObject.destroy(self)
end

function Structure:readVoxel( position )
    return self.voxelVolume:readVoxel(position)
end

function Structure:writeVoxel( position, voxel )
    local size = self.size
    assert(size, 'Structure size has not been defined yet.')

    local origin = self.origin

    assert(position:componentsGreaterOrEqualTo(origin) and
           position:componentsLesserThan(origin+size),
           'Writes are only allowed within the structures bounds.')

    return self.voxelVolume:writeVoxel(position, voxel)
end

function Structure:clear()
    local size = self.size
    assert(size, 'Structure size has not been defined yet.')

    local min = self.origin
    local max = min + size - Vec(1, 1, 1)
    local emptyVoxel = Voxel()

    for z = min[3], max[3] do
    for y = min[2], max[2] do
    for x = min[1], max[1] do
        local position = Vec(x, y, z)
        self:writeVoxel(position, emptyVoxel)
    end
    end
    end
end

function Structure:write()
    local id = self.class.id
    assert(id, 'Structure has no id assigned yet.  Forgot to register it in the structure dictionary?')

    local size = self.size
    assert(size, 'Structure size has not been defined yet.')

    local originVoxel = Voxel()
    voxelAccessor:write(originVoxel, 'id', id)
    voxelAccessor:write(originVoxel, 'size_x', size[1])
    voxelAccessor:write(originVoxel, 'size_y', size[2])
    voxelAccessor:write(originVoxel, 'size_z', size[3])
end

function Structure:read()
    local originVoxel = self:readVoxel(self.origin)
    self.size = Vec(voxelAccessor:read(originVoxel, 'size_x'),
                    voxelAccessor:read(originVoxel, 'size_y'),
                    voxelAccessor:read(originVoxel, 'size_z'))
end

---
-- Other structures that are inside this new AABB are destroyed.
-- Raises an error if they are not completly contained.
--
-- ^- What if the structure is shrunk?
--
function Structure:setSize( size )
    assert(Vec:isInstance(size), 'Size must be a vector.')
    assert(size:componentsGreaterOrEqualTo(minSize), 'Size is smaller than the minimum allowed size.')
    assert(size:componentsLesserOrEqualTo(maxSize), 'Size is larger than the maximum allowed size.')
    assert(size == Vec(1, 1, 1), 'Structures larger than 1,1,1 are not supported yet.') -- TODO!
    self.size = size
end

function Structure:generateModels( chunkBuilder )
    -- Dummy function.  This is meant to be overridden in child classes.
end


return Structure
