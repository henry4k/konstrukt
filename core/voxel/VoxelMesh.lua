--- @classmod core.voxel.VoxelMesh
-- Used by @{core.voxel.MeshChunkGenerator} to generate geometry for voxels.


local class         = require 'middleclass'
local Object        = class.Object
local isInteger     = math.isInteger
local Material      = require 'core/graphics/GraphicalMaterial'
local VoxelAccessor = require 'core/voxel/VoxelAccessor'


local VoxelMesh = class('core/voxel/VoxelMesh')

---
-- @param[type=table] t
-- Must contain the associated @{core.graphics.GraphicalMaterial} in the key
-- `material`.
-- Must contain the associated @{core.voxel.VoxelAccessor} in the key
-- `voxelAccessor`.
function VoxelMesh:initialize( t )
    assert(Object.isInstanceOf(t.material, Material),
           'Must be called with a material.')
    assert(Object.isInstanceOf(t.voxelAccessor, VoxelAccessor),
           'Must be called with a material.')
    self.material      = t.material
    self.voxelAccessor = t.voxelAccessor
    self.bitConditions = {}
end

function VoxelMesh:destroy()
end

function VoxelMesh:_addToGenerator( generator )
    error('Implementation missing.')
end

--- Geometry is only generated for voxels, which match all bit conditions of a
--- voxel mesh.
--
-- @param[type=number] offset
-- Position in the bit field.
--
-- @param[type=number] length
-- Amount of bits extracted for comparision.
--
-- @param[type=number] value
-- Value to which the bits are compared to.
function VoxelMesh:_addBitCondition( offset, length, value )
    assert(isInteger(offset), 'Offset must be an integer.')
    assert(isInteger(length), 'Length must be an integer.')
    assert(isInteger(value),  'Value must be an integer.')
    table.insert(self.bitConditions, {offset, length, value})
end

--- Geometry is only generated for voxels, which match all bit conditions of a
--- voxel mesh.
--
-- @param[type=string] maskName
-- Which mask of the voxel accessor shall be used.
--
-- @param[type=number] value
-- Value to which the bits are compared to.
function VoxelMesh:addBitCondition( maskName, value )
    local mask = self.voxelAccessor:getMask(maskName)
    assert(mask, 'No such mask defined in the voxel accessor.')
    self:_addBitCondition(mask.bitPosition, mask.bitCount, value)
end


return VoxelMesh
