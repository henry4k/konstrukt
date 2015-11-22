--- @classmod core.voxel.BlockVoxelMesh
--- A voxel mesh, which defines geometry for each side of a cube.


local class      = require 'middleclass'
local Object     = class.Object
local Matrix4    = require 'core/Matrix4'
local MeshBuffer = require 'core/graphics/MeshBuffer'
local VoxelMesh  = require 'core/voxel/VoxelMesh'
local CreateBlockVoxelMesh = ENGINE.CreateBlockVoxelMesh


local nameToIndex =
{
    ['center'] = 1,
    ['+x']     = 2,
    ['-x']     = 3,
    ['+y']     = 4,
    ['-y']     = 5,
    ['+z']     = 6,
    ['-z']     = 7
}

local BlockVoxelMesh = class('core/voxel/BlockVoxelMesh', VoxelMesh)

---
-- @param[type=table] t
-- Use the key `isTransparent` to classify the voxel mesh as transparent.
-- I.e. the geometry is rendered transparent later or it has holes, through
-- which one can look behind the voxel.
function BlockVoxelMesh:initialize( t )
    VoxelMesh.initialize(self, t)
    self.isTransparent = t.isTransparent or false
    self.meshBuffers = {}
    self.meshBufferTransformations = {}
end

function BlockVoxelMesh:_addToGenerator( generator )
    local materialId = generator:_getMaterialId(self.material)
    CreateBlockVoxelMesh(generator.handle,
                         materialId,
                         self.bitConditions,
                         self.isTransparent,
                         self.meshBuffers,
                         self.meshBufferTransformations)
end

--- Define the geometry used for a cube side.
--
-- @param name
-- Which cube side is addressed.  These have the following names:
--
-- - +x: Positive direction on X axis
-- - -x: Negative direction on X axis
-- - +y: Positive direction on Y axis
-- - -y: Negative direction on Y axis
-- - +z: Positive direction on Z axis
-- - -z: Negative direction on Z axis
-- - center: Used if any cube side is visible
function BlockVoxelMesh:setMeshBuffer( name, buffer, transformation )
    assert(nameToIndex[name], 'Invalid buffer name.')
    assert(Object.isInstanceOf(buffer, MeshBuffer),
           'Must be called with a mesh buffer.')
    assert(not transformation or
           Object.isInstanceOf(transformation, Matrix4),
           'Transformation must be a matrix or nil.')
    local index = nameToIndex[name]
    self.meshBuffers[index] = buffer
    self.meshBufferTransformations[index] = transformation
end


return BlockVoxelMesh
