--- @classmod core.voxel.MeshChunkGenerator
--- Generates a graphical representation of a voxel set.
-- The chunk generator uses voxel meshes to determine what geometry
-- each voxel needs.


local engine      = require 'engine'
local class       = require 'middleclass'
local Object      = class.Object
local Scheduler   = require 'core/Scheduler'
local Mesh        = require 'core/graphics/Mesh'
local VoxelMesh   = require 'core/voxel/VoxelMesh'
local VoxelVolume = require 'core/voxel/VoxelVolume'


local MeshChunkGenerator = class('core/voxel/MeshChunkGenerator')

function MeshChunkGenerator:initialize()
    self.handle = Scheduler.awaitCall(engine.CreateMeshChunkGenerator)
    self.nextMaterialId = 1
    self.materialToId = {}
    self.idToMaterial = {}
end

function MeshChunkGenerator:destroy()
    assert(self.handle)
    Scheduler.blindCall(engine.DestroyMeshChunkGenerator, self.handle)
    self.handle = nil
end

function MeshChunkGenerator:_getMaterialId( material )
    local id = self.materialToId[material]
    if not id then
        id = self.nextMaterialId
        self.nextMaterialId = self.nextMaterialId + 1
        self.materialToId[material] = id
        self.idToMaterial[id]       = material
    end
    return id
end

---
-- @param[type=core.voxel.VoxelMesh] voxelMesh
function MeshChunkGenerator:addVoxelMesh( voxelMesh )
    assert(Object.isInstanceOf(voxelMesh, VoxelMesh),
           'Must be called with a voxel mesh.')
    voxelMesh:_addToGenerator(self)
end


--- Generates a #MeshChunk from a section of a voxel volume.
--
-- @return[type=table]
-- A map, which contains a mesh for each material encountered during
-- generation.
--
function MeshChunkGenerator:generateChunk( voxelVolume, start, size )
    assert(Object.isInstanceOf(voxelVolume, VoxelVolume),
           'Must be called with a voxel volume.')
    assert(#start == 3 and #size == 3,
           'Start and size vectors must have 3 components.')
    local meshes, materialIds = Scheduler.awaitCall(engine.GenerateMeshChunk,
        self.handle,
        voxelVolume.handle,
        start[1],
        start[2],
        start[3],
        size[1],
        size[2],
        size[3])
    assert(#meshes == #materialIds)
    local result = {}
    for i = 1, #meshes do
        local id = materialIds[i]
        local material = self.idToMaterial[id]
        assert(material)
        result[material] = Mesh(meshes[i])
    end
    return result
end


return MeshChunkGenerator
