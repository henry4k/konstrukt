--- @classmod core.voxel.Chunk
--
-- Represents a part of the static game world.
-- - contains models, solids, etc. of the static wold in the given area


local format = string.format
local class  = require 'middleclass'
local Mat4   = require 'core/Matrix4'


local Chunk = class('core/voxel/Chunk')

--- Generates a chunk id from chunk coordinates.
function Chunk.static:idFromChunkCoords( x, y, z )
    return format('%d,%d,%d', x, y, z)
end

function Chunk:initialize()
    self.materialModels = {}
end

function Chunk:destroy()
    -- destroy created resource here (models probably)
    for _, model in pairs(self.materialModels) do
        model:destroy()
    end
    self.materialModels = nil
end

--- Updates the chunks world representation
function Chunk:update( voxelVolume, start, size, modelWorld, meshChunkGenerator )
    local materialModels = self.materialModels
    local materialMeshMap =
        meshChunkGenerator:generateChunk(voxelVolume, start, size)

    -- Create or update models:
    for material, mesh in pairs(materialMeshMap) do
        local model = materialModels[material]
        if model then
            model:setMesh(mesh)
        else
            model = modelWorld:createModel()
            model:setTransformation(Mat4():translate(start+size*0.5))
            material:updateModel(model)
            model:setMesh(mesh)
            materialModels[material] = model
        end
        mesh:destroy() -- Lua doesn't need it anymore - the model references it now.
    end

    -- Destroy unused models:
    for material, model in pairs(materialModels) do
        local mesh = materialMeshMap[material]
        if not mesh then
            model:destroy()
            materialModels[material] = nil
        end
    end
end


return Chunk
