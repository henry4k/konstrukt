--- @classmod core.voxel.Chunk
--
-- Represents a part of the static game world.
-- - contains models, solids, etc. of the static wold in the given area


local format = string.format
local class  = require 'middleclass'


local Chunk = class('core/voxel/Chunk')

--- Generates a chunk id from chunk coordinates.
function Chunk.static:idFromChunkCoords( x, y, z )
    return format('%d,%d,%d', x, y, z)
end

function Chunk:initialize()
    self.materialMap = {}
end

function Chunk:destroy()
    -- destroy created resource here (meshes and models probably)
    for _, entry in pairs(self.materialMap) do
        entry.model:destroy()
        entry.mesh:destroy()
    end
    self.materialMap = nil
end

--- Updates the chunks world representation
function Chunk:update( voxelVolume, start, size, modelWorld, meshChunkGenerator )
    local materialMap = self.materialMap
    local materialMeshMap =
        meshChunkGenerator:generateChunk(voxelVolume, start, size)

    -- Create or update models:
    for material, mesh in pairs(materialMeshMap) do
        local materialEntry = materialMap[material]
        if materialEntry then
            materialEntry.model:setMesh(mesh)
            materialEntry.mesh:destroy()
            materialEntry.mesh = mesh
        else
            local model = modelWorld:createModel()
            material:updateModel(model)
            model:setMesh(mesh)
            materialMap[material] = { model = model, mesh = mesh }
        end
    end

    -- Destroy unused models:
    for material, materialEntry in pairs(materialMap) do
        local mesh = materialMeshMap[material]
        if not mesh then
            materialEntry.model:destroy()
            materialEntry.mesh:destroy()
            materialMap[material] = nil
        end
    end
end


return Chunk
