local assert        = assert
local tostring      = tostring
local class         = require 'middleclass'
local Object        = class.Object
local Vec           = require 'core/Vector'
local Structure     = require 'core/world/Structure'
local Voxel         = require 'core/world/Voxel'
local VoxelVolume   = require 'core/world/VoxelVolume'
local VoxelAccessor = require 'core/world/VoxelAccessor'


-- AabbStructures have an origin point and a size (this is called AABB).
-- This defines which voxel belong to the structure.
local AabbStructure = class('core/world/AabbStructure', Structure)

local minSize = Vec(1, 1, 1)
local maxSize = Vec(15, 15, 15) -- 2^4 - 1

local voxelAccessor = VoxelAccessor(Structure.voxelAccessor)
voxelAccessor:addMask('size_x', 4)
voxelAccessor:addMask('size_y', 4)
voxelAccessor:addMask('size_z', 4)
AabbStructure.static.voxelAccessor = voxelAccessor

function AabbStructure:initialize( ... )
    Structure.initialize(self, ...)

    self.size = nil
end

function AabbStructure:writeVoxel( position, voxel )
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
    local size = self.size
    assert(size, 'Structure size has not been defined yet.')

    local originVoxel = Voxel()
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
