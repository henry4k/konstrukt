--- @classmod core.voxel.VoxelMesh
-- Used by @{core.voxel.MeshChunkGenerator} to generate geometry for voxels.


local class     = require 'middleclass'
local Object    = class.Object
local isInteger = math.isInteger
local Material  = require 'core/graphics/GraphicalMaterial'
local Voxel     = require 'core/voxel/Voxel'


local VoxelMesh = class('core/voxel/VoxelMesh')

---
-- @param[type=table] t
-- Must contain the associated @{core.graphics.GraphicalMaterial} in the key
-- `material`.
-- Must contain the associated @{core.voxel.Voxel} class in the key
-- `voxelClass`.
function VoxelMesh:initialize( t )
    assert(Object.isInstanceOf(t.material, Material),
           'Must be called with a material.')
    assert(Object.isSubclassOf(t.voxelClass, Voxel),
           'Must be called with a voxel class.')
    self.material      = t.material
    self.voxelClass    = t.voxelClass
    self.bitConditions = {}

    assert(self.voxelClass.id, 'Voxel class has no id yet.  Forgot to register it?')
    self:addAttributeCondition('id', self.voxelClass.id)
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
function VoxelMesh:addBitCondition( offset, length, value )
    assert(isInteger(offset), 'Offset must be an integer.')
    assert(isInteger(length), 'Length must be an integer.')
    assert(isInteger(value),  'Value must be an integer.')
    table.insert(self.bitConditions, {offset, length, value})
end

--- Geometry is only generated for voxels, which match all bit conditions of a
--- voxel mesh.
--
-- @param[type=string] attributeName
-- Which attribute mask of the voxel class shall be used.
--
-- @param[type=number] value
-- Value to which the bits are compared to.
function VoxelMesh:addAttributeCondition( attributeName, value )
    local mask = self.voxelClass:getAttributeMask(attributeName)
    assert(mask, 'No such attribute mask has been defined in the voxel class.')
    self:addBitCondition(mask.bitPosition, mask.bitCount, value)
end


return VoxelMesh
