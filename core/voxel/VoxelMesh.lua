--- @classmod core.voxel.VoxelMesh
-- Used by @{core.voxel.MeshChunkGenerator} to generate geometry for voxels.


local class     = require 'middleclass'
local Object    = class.Object
local isInteger = math.isInteger
local Material  = require 'core/graphics/GraphicalMaterial'


local VoxelMesh = class('core/voxel/VoxelMesh')

---
-- @param[type=table] t
-- Must contain the associated @{core.graphics.GraphicalMaterial} in the key
-- `material`.
function VoxelMesh:initialize( t )
    assert(Object.isInstanceOf(t.material, Material),
           'Must be called with a material.')
    self.material      = t.material
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
function VoxelMesh:addBitCondition( offset, length, value )
    assert(isInteger(offset), 'Offset must be an integer.')
    assert(isInteger(length), 'Length must be an integer.')
    assert(isInteger(value),  'Value must be an integer.')
    table.insert(self.bitConditions, {offset, length, value})
end


return VoxelMesh
