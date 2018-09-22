--- @classmod core.voxel.BlockVoxelMesh
--- A voxel mesh, which defines geometry for each side of a cube.


local engine     = require 'engine'
local class      = require 'middleclass'
local Object     = class.Object
local Scheduler  = require 'core/Scheduler'
local Vec        = require 'core/Vector'
local Mat4       = require 'core/Matrix4'
local MeshBuffer = require 'core/graphics/MeshBuffer'
local VoxelMesh  = require 'core/voxel/VoxelMesh'


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

local halfPi = math.pi / 2.0

local defaultTransformations =
{
    ['center'] = Mat4(),
    ['+x']     = Mat4():rotate(-halfPi, Vec(0,1,0)),
    ['-x']     = Mat4():rotate( halfPi, Vec(0,1,0)),
    ['+y']     = Mat4():rotate( halfPi, Vec(1,0,0)),
    ['-y']     = Mat4():rotate(-halfPi, Vec(1,0,0)),
    ['+z']     = Mat4(),
    ['-z']     = Mat4():rotate(math.pi, Vec(0,1,0))
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
    Scheduler.blindCall(engine.CreateBlockVoxelMesh,
        generator.handle,
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
--
-- @param[type=core.Matrix4,opt] transformation
-- Is applied the buffer.  It defaults to a transformation, which expects each
-- mesh buffer to be oriented in +Z direction.
function BlockVoxelMesh:setMeshBuffer( name, buffer, transformation )
    assert(nameToIndex[name], 'Invalid buffer name.')
    assert(Object.isInstanceOf(buffer, MeshBuffer),
           'Must be called with a mesh buffer.')
    assert(not transformation or
           Object.isInstanceOf(transformation, Mat4),
           'Transformation must be a matrix or nil.')
    local index = nameToIndex[name]
    self.meshBuffers[index] = buffer.handle
    if transformation then
        self.meshBufferTransformations[index] = transformation.handle
    else
        self.meshBufferTransformations[index] = defaultTransformations[name].handle
    end
end


return BlockVoxelMesh
