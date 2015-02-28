--- @classmod core.world.MultiVoxelStructure
--- @alias MVStructure
--- Structures that can span multiple voxels.
--
-- A multi voxel structure owns all voxels in an AABB volume.
--
-- It has an origin point and a size, wich defines an AABB.
-- This defines which voxel belong to the structure.
--
-- Extends @{Structure}.

local assert        = assert
local tostring      = tostring
local class         = require 'middleclass'
local Object        = class.Object
local Vec           = require 'core/Vector'
local WorldObject   = require 'core/world/WorldObject'
local Voxel         = require 'core/world/Voxel'
local VoxelVolume   = require 'core/world/VoxelVolume'
local VoxelAccessor = require 'core/world/VoxelAccessor'
local Structure     = require 'core/world/Structure'


local MVStructure = class('core/world/MultiVoxelStructure', Structure)

local minSize = Vec(1, 1, 1)
local maxSize = Vec(15, 15, 15) -- 2^4 - 1

local voxelAccessor = VoxelAccessor(Structure.static.voxelAccessor)
voxelAccessor:addMask('isOrigin', 1)
voxelAccessor:addMask('sizeX', 4)
voxelAccessor:addMask('sizeY', 4)
voxelAccessor:addMask('sizeZ', 4)
MVStructure.static.voxelAccessor = voxelAccessor

---
-- @param voxelVolume
-- @param position
--
function MVStructure:initialize( voxelVolume, position )
    Structure.initialize(self, voxelVolume, position)

    self.origin = origin
    self.size = nil
end

--[[ TODO: Not sure whether MVStructres should work on voxel level.

function MVStructure:readVoxel( position )
    return self.voxelVolume:readVoxel(position)
end

function MVStructure:writeVoxel( position, voxel )
    local size = self.size
    assert(size, 'Structure size has not been defined yet.')

    local origin = self.origin

    assert(position:componentsGreaterOrEqualTo(origin) and
           position:componentsLesserThan(origin+size),
           'Writes are only allowed within the structures bounds.')

    return self.voxelVolume:writeVoxel(position, voxel)
end
]]

function MVStructure:clear()
    Structure.clear(self)

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

function MVStructure:write()
    local size = self.size
    assert(size, 'Structure size has not been defined yet.')

    local voxel = self.voxel
    voxelAccessor:write(voxel, 'sizeX', size[1])
    voxelAccessor:write(voxel, 'sizeY', size[2])
    voxelAccessor:write(voxel, 'sizeZ', size[3])

    Structure.write(self)
end

function MVStructure:read()
    Structure.read(self)

    local voxel = self.voxel
    self.size = Vec(voxelAccessor:read(voxel, 'sizeX'),
                    voxelAccessor:read(voxel, 'sizeY'),
                    voxelAccessor:read(voxel, 'sizeZ'))
end

---
-- Other structures that are inside this new AABB are destroyed.
-- Raises an error if they are not completly contained.
--
-- ^- What if the structure is shrunk?
--
function MVStructure:setSize( size )
    assert(Vec:isInstance(size), 'Size must be a vector.')
    assert(size:componentsGreaterOrEqualTo(minSize), 'Size is smaller than the minimum allowed size.')
    assert(size:componentsLesserOrEqualTo(maxSize), 'Size is larger than the maximum allowed size.')
    assert(size == Vec(1, 1, 1), 'Structures larger than 1,1,1 are not supported yet.') -- TODO!
    self.size = size
end


return MVStructure
